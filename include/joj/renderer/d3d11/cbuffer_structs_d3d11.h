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
    };

    struct CBPerFrame
    {
        joj::DirectionalLight dir_lights[3];
        joj::JFloat3 eye_posw;

        f32 fog_start;
        f32 fog_range;
        f32 fog_color;

        i32 light_count;
        b8 use_texture;
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_CBUFFER_STRUCTS_H