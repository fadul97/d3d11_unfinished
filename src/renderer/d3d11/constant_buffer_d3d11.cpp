#include "renderer/d3d11/constant_buffer_d3d11.h"

#if JPLATFORM_WINDOWS

#include "logger.h"

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

joj::ErrorCode joj::D3D11ConstantBuffer::create(ID3D11Device* device)
{
    if (m_init_data.pSysMem != nullptr)
    {
        if (device->CreateBuffer(
            &m_cbd,
            &m_init_data,
            &m_buffer) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDERER_D3D11_CONSTANT_BUFFER_CREATION,
                "Failed to create constant buffer.");
            return ErrorCode::ERR_RENDERER_D3D11_CONSTANT_BUFFER_CREATION;
        }

    }
    else
    {
        if (device->CreateBuffer(
            &m_cbd,
            nullptr,
            &m_buffer) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDERER_D3D11_CONSTANT_BUFFER_CREATION,
                "Failed to create constant buffer.");
            return ErrorCode::ERR_RENDERER_D3D11_CONSTANT_BUFFER_CREATION;
        }
    }

    return ErrorCode::OK;
}

void joj::D3D11ConstantBuffer::bind_to_vertex_shader(ID3D11DeviceContext* device_context,
    u32 start_slot, u32 num_buffers)
{
    device_context->VSSetConstantBuffers(start_slot, num_buffers, &m_buffer);
}

void joj::D3D11ConstantBuffer::bind_to_pixel_shader(ID3D11DeviceContext* device_context,
    u32 start_slot, u32 num_buffers)
{
    device_context->PSSetConstantBuffers(start_slot, num_buffers, &m_buffer);
}

#endif // JPLATFORM_WINDOWS