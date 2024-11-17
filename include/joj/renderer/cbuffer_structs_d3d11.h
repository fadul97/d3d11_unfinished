#ifndef JOJ_D3D11_CBUFFER_H
#define JOJ_D3D11_CBUFFER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

namespace joj
{
    struct CBPerObject
    {
        joj::JFloat4x4 world = joj::float4x4_identity();
        joj::JFloat4x4 world_inv_transpose = joj::float4x4_identity();
        joj::JFloat4x4 wvp = joj::float4x4_identity();
        joj::SimpleMaterial material;
    };

    struct CBPerFrame
    {
        joj::DirectionalLight dir_light;
        joj::PointLight point_light;
        joj::SpotLight spot_light;
        joj::JFloat3 eye_posw;
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_CBUFFER_H