#include "renderer/d3d11/index_buffer_d3d11.h"

#if JPLATFORM_WINDOWS

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

#endif // JPLATFORM_WINDOWS