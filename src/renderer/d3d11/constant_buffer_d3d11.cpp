#include "renderer/d3d11/constant_buffer_d3d11.h"

joj::D3D11ConstantBuffer::D3D11ConstantBuffer()
{
    m_cbd = { 0 };
    m_buffer = nullptr;
    m_init_data = { 0 };
}

joj::D3D11ConstantBuffer::~D3D11ConstantBuffer()
{
    if (m_buffer != nullptr)
    {
        m_buffer->Release();
        m_buffer = nullptr;
    }
}

void joj::D3D11ConstantBuffer::setup(const u32 byte_width, const void* data)
{
    m_cbd.ByteWidth = byte_width;
    m_cbd.Usage = D3D11_USAGE_DYNAMIC;
    m_cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    if (data != nullptr)
        m_init_data.pSysMem = data;
}
