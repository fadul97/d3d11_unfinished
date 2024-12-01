#include "renderer/d3d11/sampler_state_d3d11.h"

#if JPLATFORM_WINDOWS

#include "logger.h"

joj::D3D11SamplerState::D3D11SamplerState()
    : m_anisotropic(nullptr)
{
}

joj::D3D11SamplerState::~D3D11SamplerState()
{
    // Release Anisotropic Sampler State
    if (m_anisotropic)
    {
        m_anisotropic->Release();
        m_anisotropic = nullptr;
    }
}

joj::ErrorCode joj::D3D11SamplerState::create_anisotropic_state(ID3D11Device* device)
{
    if (m_anisotropic != nullptr)
        return ErrorCode::OK;

    D3D11_SAMPLER_DESC anisotropic_desc = {};
    anisotropic_desc.Filter = D3D11_FILTER_ANISOTROPIC;
    anisotropic_desc.MaxAnisotropy = 4;
    anisotropic_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    anisotropic_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    anisotropic_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    anisotropic_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    anisotropic_desc.MinLOD = 0;
    anisotropic_desc.MaxLOD = D3D11_FLOAT32_MAX;

    if (device->CreateSamplerState(&anisotropic_desc, &m_anisotropic) != S_OK)
    {
        JERROR(ErrorCode::ERR_SAMPLER_STATE_D3D11_CREATION, "Failed to create Sampler State.");
        return ErrorCode::ERR_SAMPLER_STATE_D3D11_CREATION;
    }

    return ErrorCode::OK;
}

void joj::D3D11SamplerState::bind_anisotropic_state(ID3D11DeviceContext* device_context, u32 start_slot, u32 num_samplers)
{
    device_context->PSSetSamplers(start_slot, num_samplers, &m_anisotropic);
}

#endif // JPLATFORM_WINDOWS