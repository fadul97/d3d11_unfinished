#include "renderer/d3d11/sprite_sheet_d3d11.h"

#if JPLATFORM_WINDOWS

#include <logger.h>
#include <renderer/d3d11/DDSTextureLoader11.h>

joj::D3D11SpriteSheet::D3D11SpriteSheet()
    : m_srv(nullptr), m_width(0), m_height(0),
    m_cell_width(0), m_cell_height(0),
    m_rows(0), m_columns(0)
{
}

joj::D3D11SpriteSheet::~D3D11SpriteSheet()
{
    if (m_srv)
    {
        m_srv->Release();
        m_srv = nullptr;
    }
}

void joj::D3D11SpriteSheet::bind(ID3D11DeviceContext* device_context, u32 start_slot, u32 num_views)
{
    device_context->PSSetShaderResources(start_slot, num_views, &m_srv);
}

void joj::D3D11SpriteSheet::unbind(ID3D11DeviceContext* device_context)
{
    ID3D11ShaderResourceView* null_srv[1] = { nullptr };
    device_context->PSSetShaderResources(0, 1, null_srv);
}

joj::ErrorCode joj::D3D11SpriteSheet::create(ID3D11Device* device, const wchar_t* filepath,
    u32 width, u32 height, u32 rows, u32 columns)
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

    m_width = width;
    m_height = height;
    m_rows = rows;
    m_columns = columns;
    m_cell_width = width / columns;
    m_cell_height = height / rows;

    JDEBUG("Cell = %dx%d", m_cell_width, m_cell_height);

    return ErrorCode::OK;
}

void joj::D3D11SpriteSheet::destroy()
{
    if (m_srv)
    {
        m_srv->Release();
        m_srv = nullptr;
    }
}

#endif // JPLATFORM_WINDOWS