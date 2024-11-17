// TODO: Better name for this file (?)

#ifndef JOJ_D3D11_CBUFFER_STRUCTS_H
#define JOJ_D3D11_CBUFFER_STRUCTS_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "math/jmath.h"
#include "systems/light/light.h"
#include "systems/material/material.h"

namespace joj
{
    struct CBPerObject
    {
        joj::JFloat4x4 world = joj::float4x4_identity();
        joj::JFloat4x4 world_inv_transpose = joj::float4x4_identity();
        joj::JFloat4x4 wvp = joj::float4x4_identity();
        joj::JFloat4x4 tex_transform = joj::float4x4_identity();
        joj::SimpleMaterial material;
        joj::JFloat2 cell_size = { 0.0f, 0.0f };    // Cell size in atlas
        i32 current_frame = 0;
        f32 num_columns = 0;                        // Column number in atlas
    };

    struct CBPerFrame
    {
        joj::DirectionalLight dir_lights[3];

        alignas(16) joj::JFloat3 eye_posw;    // Aligned to 16 bytes
        f32 pad1;                             // Pad for next member

        f32 fog_start;                        // 4 bytes
        f32 fog_range;                        // 4 bytes
        alignas(16) joj::JFloat4 fog_color;   // Aligned to 4 bytes

        i32 light_count;                      // 4 bytes
        b32 use_texture;                      // 4 bytes
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_CBUFFER_STRUCTS_H