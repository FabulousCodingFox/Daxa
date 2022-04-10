#if !defined(WORLD_NOISE_HLSL)
#define WORLD_NOISE_HLSL

#include "utils/noise.hlsl"
#include "block_info.hlsl"

float terrain_noise(float3 pos) {
    FractalNoiseConfig noise_conf = {
        /* .amplitude   = */ 1.0f,
        /* .persistance = */ 0.4f,
        /* .scale       = */ 0.01f,
        /* .lacunarity  = */ 2,
        /* .octaves     = */ 6,
    };
    return fractal_noise(pos, noise_conf) - 1.5; // + (pos.y - 120) * 0.015;
}

float biome_noise(float3 pos) {
    FractalNoiseConfig noise_conf = {
        /* .amplitude   = */ 1.0f,
        /* .persistance = */ 0.4f,
        /* .scale       = */ 0.001f,
        /* .lacunarity  = */ 4,
        /* .octaves     = */ 4,
    };
    return fractal_noise(float3(pos.x, 0, pos.z) + 1000, noise_conf);
}

float underworld_noise(float3 pos) {
    FractalNoiseConfig noise_conf = {
        /* .amplitude   = */ 1.0f,
        /* .persistance = */ 0.5f,
        /* .scale       = */ 0.002f,
        /* .lacunarity  = */ 2,
        /* .octaves     = */ 6,
    };
    return fractal_noise(pos, noise_conf) + abs(400 - pos.y) * 0.015 - 1.0;
}

float cave_noise(float3 pos) {
    FractalNoiseConfig noise_conf = {
        /* .amplitude   = */ 1.0f,
        /* .persistance = */ 0.4f,
        /* .scale       = */ 0.02f,
        /* .lacunarity  = */ 2,
        /* .octaves     = */ 4,
    };
    return fractal_noise(pos, noise_conf) + abs(220 - pos.y) * 0.01 - 0.5;
}

struct WorldgenState {
    float t_noise;
    float b_noise;
    float u_noise;
    float c_noise;

    float r;
    float r_xz;

    BlockID block_id;
    BiomeID biome_id;
};

WorldgenState get_worldgen_state(float3 pos) {
    WorldgenState result;
    result.t_noise = terrain_noise(pos);
    result.b_noise = biome_noise(pos);
    result.u_noise = underworld_noise(pos);
    result.c_noise = cave_noise(pos);
    result.r = rand(pos);
    result.r_xz = rand(float3(pos.x, 0, pos.z));
    return result;
}

#endif