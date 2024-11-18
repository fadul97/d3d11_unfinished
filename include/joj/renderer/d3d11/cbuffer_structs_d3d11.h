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

        b32 use_texture;
        b32 alpha_clip;
        b32 fog_enabled;
        f32 padding;
    };

    struct CBPerFrame
    {
        joj::DirectionalLight dir_lights[3];

        alignas(16) joj::JFloat3 eye_posw;    // Aligned to 16 bytes
        f32 pad1;                             // Pad for next member

        f32 fog_start;
        f32 fog_range;
        alignas(16) joj::JFloat4 fog_color;

        i32 light_count;
        i32 pad2[3];                          // 12 bytes (padding for 16-byte alignment)
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_CBUFFER_STRUCTS_H