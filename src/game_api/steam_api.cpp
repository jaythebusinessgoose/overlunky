#include "steam_api.hpp"

#include <array>     // for array, _Array_const_iterator
#include <detours.h> // for DetourAttach, DetourTransactionBegin

#include "memory.hpp" // for vtable_find
#include "script/events.hpp"
#include "search.hpp" // for get_address
#include "strings.hpp"
#include "vtable_hook.hpp" // for get_hook_function, register_hook_function
#include "level_api.hpp"

class ISteamUserStats;

class ISteamUserStats* get_steam_user_stats()
{
    using GetSteamUserStatsFun = void(ISteamUserStats**);
    static GetSteamUserStatsFun* get_steam_user_stats_impl = *(GetSteamUserStatsFun**)get_address("get_steam_user_stats");

    ISteamUserStats* steam_user_stats{nullptr};
    get_steam_user_stats_impl(&steam_user_stats);
    return steam_user_stats;
}

void enable_steam_achievements()
{
    ISteamUserStats* steam_user_stats = get_steam_user_stats();
    if (steam_user_stats != nullptr && get_hook_function((void***)steam_user_stats, 0x7))
    {
        unregister_hook_function((void***)steam_user_stats, 0x7);
    }
}

void disable_steam_achievements()
{
    ISteamUserStats* steam_user_stats = get_steam_user_stats();
    if (steam_user_stats != nullptr && !get_hook_function((void***)steam_user_stats, 0x7))
    {
        constexpr auto nop_set_achievement = [](ISteamUserStats*, const char*)
        {
            return true;
        };

        using ISteamUserStats_SetAchievement = bool(ISteamUserStats*, const char*);
        register_hook_function(
            (void***)steam_user_stats,
            0x7,
            (ISteamUserStats_SetAchievement*)nop_set_achievement);
    }
}

bool get_steam_achievement(const char* achievement_id, bool* achieved)
{
    ISteamUserStats* steam_user_stats = get_steam_user_stats();
    if (steam_user_stats != nullptr)
    {
        using ISteamUserStats_GetAchievement = bool(ISteamUserStats*, const char*, bool*);
        ISteamUserStats_GetAchievement* get_achievement = *vtable_find<ISteamUserStats_GetAchievement*>(steam_user_stats, 0x6);
        return get_achievement(steam_user_stats, achievement_id, achieved);
    }
    return false;
}

bool set_steam_achievement(const char* achievement_id, bool achieved)
{
    ISteamUserStats* steam_user_stats = get_steam_user_stats();
    if (steam_user_stats != nullptr)
    {
        if (achieved)
        {
            using ISteamUserStats_SetAchievement = bool(ISteamUserStats*, const char*);
            ISteamUserStats_SetAchievement* set_achievement = *vtable_find<ISteamUserStats_SetAchievement*>(steam_user_stats, 0x7);
            return set_achievement(steam_user_stats, achievement_id);
        }
        else
        {
            using ISteamUserStats_ResetAchievement = bool(ISteamUserStats*, const char*);
            ISteamUserStats_ResetAchievement* reset_achievement = *vtable_find<ISteamUserStats_ResetAchievement*>(steam_user_stats, 0x8);
            return reset_achievement(steam_user_stats, achievement_id);
        }
    }
    return false;
}
void reset_all_steam_achievements()
{
    ISteamUserStats* steam_user_stats = get_steam_user_stats();
    if (steam_user_stats != nullptr && !get_hook_function((void***)steam_user_stats, 0x7))
    {
        using ISteamUserStats_ResetAchievement = bool(ISteamUserStats*, const char*);
        ISteamUserStats_ResetAchievement* reset_achievement = *vtable_find<ISteamUserStats_ResetAchievement*>(steam_user_stats, 0x8);

        for (auto achievement_id : g_AllAchievements)
        {
            reset_achievement(steam_user_stats, achievement_id);
        }
    }
}

using IsShopZoneFun = bool(LevelGenSystem *, uint8_t, float, float);
IsShopZoneFun* g_is_shop_zone_trampoline{nullptr};
std::function<bool(LevelGenSystem*, uint8_t, float, float)> g_is_shop_zone{nullptr};

bool is_shop_zone(LevelGenSystem *level_gen, uint8_t layer, float x, float y)
{
    auto pre = pre_is_shop_zone(level_gen, layer, x, y);
    if (pre.has_value())
        return pre.value();
    else if (g_is_shop_zone_trampoline)
        return g_is_shop_zone_trampoline(level_gen, layer, x, y);
    return false;
}

using IsActiveShopRoomFun = bool(LevelGenSystem *, uint8_t, float, float);
IsActiveShopRoomFun* g_is_active_shop_room_trampoline{nullptr};
std::function<bool(LevelGenSystem*, uint8_t, float, float)> g_is_active_shop_room{nullptr};
bool is_active_shop_room(LevelGenSystem *level_gen, uint8_t layer, float x, float y)
{
    auto pre = pre_is_active_shop_room(level_gen, layer, x, y);
    if (pre.has_value())
        return pre.value();
    else if (g_is_active_shop_room_trampoline)
        return g_is_active_shop_room_trampoline(level_gen, layer, x, y);
    return false;
}

using GetRoomownerTypeFun = uint32_t(uint8_t, float, float);
GetRoomownerTypeFun* g_get_roomowner_type_trampoline{nullptr};
std::function<uint32_t(uint8_t, float, float)> g_get_roomowner_type{nullptr};

uint32_t get_roomowner_type(uint8_t layer, float x, float y)
{
    auto pre = pre_get_roomowner_type(layer, x, y);
    if (pre.has_value())
        return pre.value();
    else if (g_get_roomowner_type_trampoline)
        return g_get_roomowner_type_trampoline(layer, x, y);
    return 0x128; // MONS_SHOPKEEPER is default.
}

using IsRoomownerAliveFun = bool(StateMemory *, uint8_t, float, float);
IsRoomownerAliveFun* g_is_roomowner_alive_trampoline{nullptr};
std::function<bool(StateMemory *, uint8_t, float, float)> g_is_roomowner_alive{nullptr};

bool is_roomowner_alive(StateMemory *state, uint8_t layer, float x, float y)
{
    auto pre = pre_is_roomowner_alive(state, layer, x, y);
    if (pre.has_value())
        return pre.value();
    else if (g_is_roomowner_alive_trampoline)
        return g_is_roomowner_alive_trampoline(state, layer, x, y);
    return false;
}

using GetFeatFun = bool(FEAT);
GetFeatFun* g_get_feat_trampoline{nullptr};
std::function<bool(FEAT)> g_get_feat{nullptr};

using SetFeatFun = void(FEAT);
SetFeatFun* g_set_feat_trampoline{nullptr};
std::function<void(FEAT)> g_set_feat{nullptr};

void set_feat_hidden(FEAT feat, bool hidden)
{
    if (--feat > 31)
        return;
    static auto offset = get_address("get_feat_hidden"sv);
    static auto mask = memory_read<uint32_t>(offset);
    if (hidden)
        mask |= (1U << feat);
    else
        mask &= ~(1U << feat);
    write_mem_recoverable("hidden_feats", offset, mask, true);
}

bool get_feat_hidden(FEAT feat)
{
    if (--feat > 31)
        return false;
    static auto offset = get_address("get_feat_hidden"sv);
    auto mask = memory_read<uint32_t>(offset);
    return (mask & (1U << feat)) > 0;
}

bool feat_unlocked(uint8_t feat)
{
    auto pre = pre_get_feat(feat + 1);
    if (pre.has_value())
        return pre.value();
    else if (g_get_feat_trampoline)
        return g_get_feat_trampoline(feat);
    return false;
}

void unlock_feat(uint8_t feat)
{
    if (!pre_set_feat(feat + 1) && g_set_feat_trampoline)
        g_set_feat_trampoline(feat);
}

std::tuple<bool, bool, const char16_t*, const char16_t*> get_feat(FEAT feat)
{
    if (--feat > 31)
        return std::make_tuple(false, false, u"", u"");

    static const STRINGID first_feat = hash_to_stringid(0x335dbbd4); // The Full Spelunky
    auto data = std::make_tuple(feat_unlocked(feat), get_feat_hidden(feat + 1), get_string(first_feat + feat), get_string(first_feat + feat + 33));
    return data;
}

void change_feat(FEAT feat, bool hidden, std::u16string_view name, std::u16string_view description)
{
    if (--feat > 31)
        return;

    static const STRINGID first_feat = hash_to_stringid(0x335dbbd4); // The Full Spelunky
    change_string(first_feat + feat, name);
    change_string(first_feat + feat + 33, description);
    // Make up your mind! Is it feats, achievements or trophies?
    if (feat == 0)
        change_string(first_feat + 32, description);

    set_feat_hidden(feat + 1, hidden);
}

void init_achievement_hooks()
{
    static bool hooked = false;
    if (!hooked)
    {
        g_get_feat_trampoline = (GetFeatFun*)get_address("get_feat"sv);
        g_set_feat_trampoline = (SetFeatFun*)get_address("set_feat"sv);
        g_is_shop_zone_trampoline = (IsShopZoneFun*)get_address("coord_inside_shop_zone"sv);
        g_is_active_shop_room_trampoline = (IsShopZoneFun*)get_address("coord_inside_active_shop_room"sv);
        g_get_roomowner_type_trampoline = (GetRoomownerTypeFun*)get_address("coord_roomowner_type"sv);
        g_is_roomowner_alive_trampoline = (IsRoomownerAliveFun*)get_address("coord_roomowner_alive"sv);

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach((void**)&g_get_feat_trampoline, &feat_unlocked);
        DetourAttach((void**)&g_set_feat_trampoline, &unlock_feat);
        DetourAttach((void**)&g_is_shop_zone_trampoline, &is_shop_zone);
        DetourAttach((void**)&g_is_active_shop_room_trampoline, &is_active_shop_room);
        DetourAttach((void**)&g_get_roomowner_type_trampoline, &get_roomowner_type);
        DetourAttach((void**)&g_is_roomowner_alive_trampoline, &is_roomowner_alive);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking feats: {}\n", error);
        }

        hooked = true;
    }
}
