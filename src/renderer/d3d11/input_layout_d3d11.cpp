#include "renderer/d3d11/input_layout_d3d11.h"

#if JPLATFORM_WINDOWS

#include "logger.h"

joj::D3D11InputLayout::D3D11InputLayout()
    : m_layout(nullptr)
{
}

joj::D3D11InputLayout::~D3D11InputLayout()
{
}

void joj::D3D11InputLayout::describe_default_geometry_layout()
{
    m_desc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
}

joj::ErrorCode joj::D3D11InputLayout::create(ID3D11Device* device, D3D11Shader& shader)
{
    if (device->CreateInputLayout(
        m_desc.data(),
        (u32)m_desc.size(),
        shader.get_vsblob()->GetBufferPointer(),
        shader.get_vsblob()->GetBufferSize(),
        &m_layout
    ) != S_OK)
    {
        JERROR(ErrorCode::ERR_D3D11_INPUT_LAYOUT_CREATION, "Failed to create Input Layout.");
        return ErrorCode::ERR_D3D11_INPUT_LAYOUT_CREATION;
    }

    return ErrorCode::OK;
}

void joj::D3D11InputLayout::bind(ID3D11DeviceContext* device_context)
{
    device_context->IASetInputLayout(m_layout);
}

#endif // JPLATFORM_WINDOWS