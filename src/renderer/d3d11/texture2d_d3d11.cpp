#include "renderer/d3d11/texture2d_d3d11.h"

#if JPLATFORM_WINDOWS

#include "engine.h"
#include "renderer/d3d11/DDSTextureLoader11.h"
#include <renderer/d3d11/WICTextureLoader.h>
#include "logger.h"
#include <locale>
#include <codecvt>

joj::D3D11Texture2D::D3D11Texture2D()
    : m_srv(nullptr), m_width(0), m_height(0)
{
}

joj::D3D11Texture2D::~D3D11Texture2D()
{
    if (m_srv)
    {
        m_srv->Release();
        m_srv = nullptr;
    }
}

void joj::D3D11Texture2D::bind(ID3D11DeviceContext* device_context, u32 start_slot, u32 num_views)
{
    device_context->PSSetShaderResources(start_slot, num_views, &m_srv);
}

void joj::D3D11Texture2D::unbind(ID3D11DeviceContext* device_context)
{
    ID3D11ShaderResourceView* null_srv[1] = { nullptr };
    device_context->PSSetShaderResources(0, 1, null_srv);
}

joj::ErrorCode joj::D3D11Texture2D::create(ID3D11Device* device,
    ID3D11DeviceContext* device_context,
    const std::wstring& filepath, ImageType type)
{
    switch (type)
    {
    case joj::ImageType::PNG:
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string str = converter.to_bytes(filepath);
        const char* filename = str.c_str();

        if (D3D11CreateTextureFromFile(
            device,
            device_context,
            filename,
            nullptr,
            &m_srv,
            m_width,
            m_height) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION,
                "Failed to create texture from PNG file '%s'.", filename);
            return ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION;
        }
    }
        break;
    case joj::ImageType::JPG:
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string str = converter.to_bytes(filepath);
        const char* filename = str.c_str();

        if (D3D11CreateTextureFromFile(
            device,
            device_context,
            filename,
            nullptr,
            &m_srv,
            m_width,
            m_height) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION,
                "Failed to create texture from JPG file '%s'.", filename);
            return ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION;
        }
    }
        break;
    case joj::ImageType::DDS:
        if (DirectX::CreateDDSTextureFromFile(
            device,
            filepath.c_str(),
            nullptr,
            &m_srv
        ) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION,
                "Failed to create texture from DDS file '%ls'.", filepath);
            return ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION;
        }
        break;
    default:
        break;
    }

    return ErrorCode::OK;
}

void joj::D3D11Texture2D::destroy()
{
    if (m_srv)
    {
        m_srv->Release();
        m_srv = nullptr;
    }
}

#endif // JPLATFORM_WINDOWS