#pragma once

#include "game_allocator.hpp"

#include <unordered_map>

template <class K, class V>
using game_unordered_map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, game_allocator<std::pair<const K, V>>>;
