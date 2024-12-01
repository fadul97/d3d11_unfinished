#include "renderer/d3d11/index_buffer_d3d11.h"

#if JPLATFORM_WINDOWS

#include <logger.h>

joj::D3D11IndexBuffer::D3D11IndexBuffer()
{
    m_ibd = { 0 };
    m_index_buffer = nullptr;
    m_init_data = { 0 };
}

joj::D3D11IndexBuffer::~D3D11IndexBuffer()
{
    // Release Vertex Buffer
    if (m_index_buffer)
    {
        m_index_buffer->Release();
        m_index_buffer = nullptr;
    }
}

void joj::D3D11IndexBuffer::setup(u32 byte_width, const void* data)
{
    // Describe Index Buffer
    m_ibd.Usage = D3D11_USAGE_IMMUTABLE;
    m_ibd.ByteWidth = byte_width;
    m_ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_ibd.CPUAccessFlags = 0;
    m_ibd.MiscFlags = 0;
    m_ibd.StructureByteStride = 0;

    if (data != nullptr)
        m_init_data.pSysMem = data;
}

joj::ErrorCode joj::D3D11IndexBuffer::create(ID3D11Device* device)
{
    if (m_init_data.pSysMem != nullptr)
    {
        if (device->CreateBuffer(
            &m_ibd,
            &m_init_data,
            &m_index_buffer) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDERER_D3D11_INDEX_BUFFER_CREATION,
                "Failed to create index buffer.");
            return ErrorCode::ERR_RENDERER_D3D11_INDEX_BUFFER_CREATION;
        }

    }
    else
    {
        if (device->CreateBuffer(
            &m_ibd,
            nullptr,
            &m_index_buffer) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDERER_D3D11_INDEX_BUFFER_CREATION,
                "Failed to create index buffer.");
            return ErrorCode::ERR_RENDERER_D3D11_INDEX_BUFFER_CREATION;
        }
    }

    return ErrorCode::OK;
}

void joj::D3D11IndexBuffer::bind(ID3D11DeviceContext* device_context,
    DXGI_FORMAT format, u32 offset)
{
    device_context->IASetIndexBuffer(m_index_buffer, format, offset);
}

#endif // JPLATFORM_WINDOWS