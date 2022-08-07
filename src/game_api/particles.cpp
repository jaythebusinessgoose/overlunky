#include "particles.hpp"

#include <algorithm>     // for sort
#include <functional>    // for equal_to
#include <list>          // for _List_iterator, _List_const_iterator
#include <new>           // for operator new
#include <type_traits>   // for move, hash
#include <unordered_map> // for unordered_map, _Umap_traits<>::allocator_type
#include <utility>       // for min, max

#include "search.hpp"  // for get_address
#include "texture.hpp" // for get_texture, Texture

ParticleDB* particle_db_ptr()
{
    static ParticleDB* addr = (ParticleDB*)get_address("particle_emitter_db");
    return addr;
}

std::uint64_t ParticleDB::get_texture()
{
    return texture->id;
}
bool ParticleDB::set_texture(std::uint32_t texture_id)
{
    if (auto* new_texture = ::get_texture(texture_id))
    {
        texture = new_texture;
        return true;
    }
    return false;
}

ParticleDB* get_particle_type(uint32_t id)
{
    static std::unordered_map<uint32_t, ParticleDB*> mapping = {};
    if (mapping.size() == 0)
    {
        uint32_t current_id = 0;
        uint32_t max_entries = 250;
        uint32_t counter = 0;
        ParticleDB* db = particle_db_ptr();
        while (counter < max_entries)
        {
            if (db->id > current_id && db->id < (current_id + 10)) // allow for gaps in the id's
            {
                mapping[db->id] = db;
                current_id = db->id;
            }
            else
            {
                break;
            }
            db++;
            counter++;
        }
    }
    if (mapping.count(id) != 0)
    {
        return mapping.at(id);
    }
    return nullptr;
}

const std::vector<ParticleEmitter>& list_particles()
{
    static std::vector<ParticleEmitter> particles = {};
    if (particles.size() == 0)
    {
        auto mapOffset = get_address("particle_emitter_list");
        std::unordered_map<std::string, uint16_t>* map = reinterpret_cast<std::unordered_map<std::string, uint16_t>*>(mapOffset);
        for (const auto& [particle_name, particle_id] : *map)
        {
            particles.emplace_back(particle_name, particle_id);
        }
        std::sort(particles.begin(), particles.end(), [](ParticleEmitter& a, ParticleEmitter& b) -> bool
                  { return a.id < b.id; });
    }
    return particles;
}

std::vector<Particle> ParticleEmitterInfo::emitted_particles()
{
    static std::vector<Particle> particles;
    particles.reserve(particle_count);

    for (uint32_t particle_index = 0; particle_index < particle_count; ++particle_index) {
        EmittedParticlesInfo particleInfos = emitted_particle_infos_type1;
        // emitted_particle_infos_type1->x_positions[particle_index]
        ParticleColor particleColor = particleInfos.colors[particle_index];

        Color color = Color(particleColor.red / 255.0f, particleColor.green / 255.0f, particleColor.blue / 255.0f, particleColor.alpha / 255.0f);
        Particle particle = {
            this,
            particle_index,
            particleInfos.x_positions[particle_index],
            particleInfos.y_positions[particle_index],
            particleInfos.unknown_x_positions[particle_index],
            particleInfos.unknown_y_positions[particle_index],
            color,
            particleInfos.widths[particle_index],
            particleInfos.heights[particle_index],
            particleInfos.x_velocities[particle_index],
            particleInfos.y_velocities[particle_index],
            particleInfos.lifetimes[particle_index],
            particleInfos.max_lifetimes[particle_index],
        };
        particles.push_back(particle);
    }
    return particles;
}

Particle ParticleEmitterInfo::emitted_particle_at(uint32_t particle_index)
{
    EmittedParticlesInfo particleInfos = emitted_particle_infos_type1;
    // emitted_particle_infos_type1->x_positions[particle_index]
    ParticleColor particleColor = particleInfos.colors[particle_index];

    Color color = Color(particleColor.red / 255.0f, particleColor.green / 255.0f, particleColor.blue / 255.0f, particleColor.alpha / 255.0f);
    return {
        this,
        particle_index,
        particleInfos.x_positions[particle_index],
        particleInfos.y_positions[particle_index],
        particleInfos.unknown_x_positions[particle_index],
        particleInfos.unknown_y_positions[particle_index],
        color,
        particleInfos.widths[particle_index],
        particleInfos.heights[particle_index],
        particleInfos.x_velocities[particle_index],
        particleInfos.y_velocities[particle_index],
        particleInfos.lifetimes[particle_index],
        particleInfos.max_lifetimes[particle_index],
    };
}

void Particle::move(float new_x, float new_y)
{
    x = new_x;
    y = new_y;
    // (*emitterInfo).x_positions[index] = x;
    // (*emitterInfo).y_positions[index] = y;
    particlesInfo().x_positions[index] = x;
    particlesInfo().y_positions[index] = y;
}

uint8_t toRGB(const float c)
{
    return static_cast<uint8_t>(std::round(255 * std::min(std::max(c, 0.0f), 1.0f)));
}

void Particle::set_color(Color new_color)
{
    color = new_color;
    // (*emitterInfo).colors[index] = {toRGB(color.r),toRGB(color.g),toRGB(color.b),toRGB(color.a)};
    particlesInfo().colors[index] = {toRGB(color.r),toRGB(color.g),toRGB(color.b),toRGB(color.a)};
}

void Particle::set_width(float new_width)
{
    width = new_width;
    // printf("here width: %f", *((emitterInfo).widths));
    particlesInfo().widths[index] = new_width;
    // *((*emitterInfo).widths) = width;
    // (*emitterInfo).widths[index] = width;
}

void Particle::set_height(float new_height)
{
    height = new_height;
    particlesInfo().heights[index] = new_height;
    // (*emitterInfo).heights[index] = height;
}