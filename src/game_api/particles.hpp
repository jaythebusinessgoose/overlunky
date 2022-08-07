#pragma once

#include <cstddef> // for size_t
#include <cstdint> // for uint32_t, uint8_t, int32_t, uint64_t
#include <string>  // for string, basic_string
#include <vector>  // for vector

#include "color.hpp"   // for Color

struct Texture;

struct ParticleDB
{
    uint32_t id;
    int32_t spawn_count_min;            // minimum amount of particles to spawn in an iteration, actual value is random between this value and spawn_count (-1 = no lower bound, uses spawn_count)
    uint32_t spawn_count;               // total amount of particles to spawn for 1 iteration (check with PETTING_PET or MOUNT_TAMED, amount of hearts shown)
    int32_t lifespan_min;               // minimum lifespan of a particle, actual value is random between this value and lifespan (-1 = no lower bound, uses lifespan)
    uint32_t lifespan;                  // amount of frames
    uint32_t sheet_id;                  // zero based index of the sprite in the texture sheet DDS file
    uint32_t animation_sequence_length; // amount of sequential sprites in the animation sequence
    uint32_t unknown7;
    uint32_t unknown8;
    float spawn_interval;       // how much time it takes to spawn the next particle
    float shrink_growth_factor; // negative makes particles shrink over time, positive grow
    float rotation_speed;
    float opacity;
    float max_velocity;    // default = FLT_MAX (no limit)
    float y_scale_related; // unknown
    float hor_scattering;
    float ver_scattering;
    float scale_x_min; // scale_x = ((scale_x - scale_x_min) * (rand(0-100)/100)) + scale_x_min
    float scale_y_min;
    float scale_x;
    float scale_y;
    float hor_deflection_1;
    float ver_deflection_1;
    float hor_deflection_2;
    float ver_deflection_2;
    float hor_velocity;
    float ver_velocity;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
    Texture* texture;
    size_t unknown29; // code pointer
    size_t unknown30; // code pointer
    uint8_t unknown31a;
    uint8_t unknown31b;
    uint8_t unknown31c;
    uint8_t unknown31d;
    float unknown32;
    float unknown33;
    bool permanent; // whether you have to constantly refresh, or whether it does so on its own (e.g. petting hearts vs dustwall)
    bool unknown35;
    bool unknown36;
    bool unknown37;
    bool invisible; // used for shockwaves and warping
    bool unknown39;
    uint8_t unknown40;
    uint8_t unknown41;
    uint32_t unknown42;

    std::uint64_t get_texture();
    bool set_texture(std::uint32_t texture_id);
};

struct ParticleEmitter
{
    std::string name;
    uint32_t id;

    ParticleEmitter(const std::string& name_, uint32_t id_)
        : name(name_), id(id_)
    {
    }
};

struct ParticleColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

struct Particle;

class ParticleEmitterInfo {
    public:
    struct EmittedParticlesInfo {
        ParticleEmitterInfo* particle_emitter;
        size_t unknown4;
        uint16_t* max_lifetimes;
        uint16_t* lifetimes;
        size_t unknown7;
        size_t unknown8;
        float* x_positions;
        float* y_positions;
        float* unknown_x_positions;
        float* unknown_y_positions;
        ParticleColor* colors;
        float* widths;
        float* heights;
        float* x_velocities;
        float* y_velocities;
        size_t unknown18;
        size_t unknown19;
        size_t unknown20;
        size_t unknown21;
        size_t unknown22;
        size_t unknown23;
    };
    ParticleDB* particle_type;
    ParticleDB* particle_type2;
    uint32_t particle_count;
    uint32_t unknown2;
    EmittedParticlesInfo emitted_particle_infos_type1;
    size_t unknown24;
    // size_t unknown25;
    // size_t unknown26;
    // size_t unknown27;
    // size_t unknown28;
    // size_t unknown29;
    // size_t unknown30;
    // size_t unknown31;
    // size_t unknown32;
    // size_t unknown33;
    // size_t unknown34;
    // size_t unknown35;
    // size_t unknown36;
    // size_t unknown37;
    // size_t unknown38;
    // size_t unknown39;
    // size_t unknown40;
    // size_t unknown41;
    // size_t unknown42;
    // size_t unknown43;
    // size_t unknown44;
    // size_t unknown45;

    EmittedParticlesInfo emitted_particle_infos_type2;

    int32_t entity_uid; // set to -1 to decouple emitter position from entity position (and move it around freely)

    float unknown46;
    float unknown47;
    float unknown48;
    float unknown49;

    float x;
    float y;
    float offset_x;
    float offset_y;

    uint8_t unknown54a;
    uint8_t unknown54b;
    uint8_t unknown54c;
    uint8_t unknown54d;
    float unknown55;
    uint32_t unknown56;
    uint32_t unknown57;
    uint32_t unknown58;
    uint32_t unknown59;
    uint32_t unknown60;

    std::vector<Particle> emitted_particles();
    Particle emitted_particle_at(uint32_t index);
};


struct Particle {
    ParticleEmitterInfo *emitterInfo;
    uint32_t index;
    float x;
    float y;
    float unknown_x_related;
    float unknown_y_related;
    Color color;
    float width;
    float height;
    float velocityx;
    float velocityy;
    uint16_t lifetime;
    uint16_t max_lifetime;

    ParticleEmitterInfo::EmittedParticlesInfo particlesInfo()
    {
        return emitterInfo->emitted_particle_infos_type1;
    }


    void move(float x, float y);
    void set_color(Color color);
    void set_width(float width);
    void set_height(float height);
};

ParticleDB* get_particle_type(uint32_t id);
const std::vector<ParticleEmitter>& list_particles();
