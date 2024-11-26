#include "renderer/d3d11/3d/vertex_buffer_d3d11.h"

#if JPLATFORM_WINDOWS

joj::D3D11VertexBuffer::D3D11VertexBuffer()
{
    m_vbd = { 0 };
    m_vertex_buffer = nullptr;
    m_init_data = { 0 };
    m_filled = false;
}

joj::D3D11VertexBuffer::~D3D11VertexBuffer()
{
    if (m_filled)
    {
        // Release Vertex Buffer
        if (m_vertex_buffer)
        {
            m_vertex_buffer->Release();
            m_vertex_buffer = nullptr;
        }
    }
}

void joj::D3D11VertexBuffer::setup(D3D11_USAGE usage, u32 cpu_access_flags, u32 byte_width, const void* data)
{
    // Describe Buffer - Resource structure
    m_vbd.ByteWidth = byte_width;
    m_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_vbd.Usage = usage;
    m_vbd.CPUAccessFlags = cpu_access_flags;
    m_vbd.MiscFlags = 0;
    m_vbd.StructureByteStride = 0;

    if (data != nullptr)
        m_init_data.pSysMem = data;

    m_filled = true;
}

void joj::D3D11VertexBuffer::cleanup()
{
    if (m_filled)
    {
        // Release Vertex Buffer
        if (m_vertex_buffer)
        {
            m_vertex_buffer->Release();
            m_vertex_buffer = nullptr;
        }
    }

    m_filled = false;
}

#endif // JPLATFORM_WINDOWS