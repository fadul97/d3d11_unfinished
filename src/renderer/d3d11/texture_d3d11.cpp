#include "renderer/d3d11/texture_d3d11.h"

#if JPLATFORM_WINDOWS

#include "engine.h"
#include "renderer/d3d11/DDSTextureLoader11.h"
#include "logger.h"

joj::D3D11Texture::D3D11Texture()
    : m_srv(nullptr)
{
}

joj::D3D11Texture::D3D11Texture(ID3D11Device* device, const wchar_t* filepath)
    : m_srv(nullptr)
{
    if (DirectX::CreateDDSTextureFromFile(
        device,
        filepath,
        nullptr,
        &m_srv
    ) != S_OK)
    {
        JERROR(ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION,
            "Failed to create DDS Texture from file '%ls'.", filepath);
    }
}

joj::D3D11Texture::~D3D11Texture()
{
    if (m_srv)
    {
        m_srv->Release();
        m_srv = nullptr;
    }
}

void joj::D3D11Texture::bind(ID3D11DeviceContext* device_context, u32 start_slot, u32 num_views)
{
    device_context->PSSetShaderResources(start_slot, num_views, &m_srv);
}

void joj::D3D11Texture::unbind(ID3D11DeviceContext* device_context)
{
    ID3D11ShaderResourceView* null_srv[1] = { nullptr };
    device_context->PSSetShaderResources(0, 1, null_srv);
}

joj::ErrorCode joj::D3D11Texture::create(ID3D11Device* device, const wchar_t* filepath)
{
    if (DirectX::CreateDDSTextureFromFile(
        device,
        filepath,
        nullptr,
        &m_srv
    ) != S_OK)
    {
        JERROR(ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION,
            "Failed to create DDS Texture from file '%ls'.", filepath);
        return ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION;
    }

    return ErrorCode::OK;
}

void joj::D3D11Texture::destroy()
{
    if (m_srv)
    {
        m_srv->Release();
        m_srv = nullptr;
    }
}

#endif // JPLATFORM_WINDOWS