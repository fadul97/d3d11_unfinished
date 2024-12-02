#include "renderer/d3d11/tileset_d3d11.h"

#if JPLATFORM_WINDOWS

#include "renderer/d3d11/WICTextureLoader.h"
#include "renderer/d3d11/DDSTextureLoader11.h"
#include <logger.h>
#include <locale>
#include <codecvt>

joj::D3D11Tileset::D3D11Tileset()
    : m_width(0), m_height(0), m_srv(nullptr)
{
}

joj::D3D11Tileset::~D3D11Tileset()
{
    if (m_srv)
    {
        m_srv->Release();
        m_srv = nullptr;
    }
}

joj::ErrorCode joj::D3D11Tileset::create(ID3D11Device* device,
    ID3D11DeviceContext* device_context,
    const std::wstring& filepath, ImageType type)
{
    if (type == ImageType::PNG)
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
    else
    {
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
    }

    return ErrorCode::OK;
}

void joj::D3D11Tileset::destroy()
{
    if (m_srv)
    {
        m_srv->Release();
        m_srv = nullptr;
    }
}

#endif // JPLATFORM_WINDOWS