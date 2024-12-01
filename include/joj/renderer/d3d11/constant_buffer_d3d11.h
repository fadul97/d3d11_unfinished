#ifndef JOJ_D3D11_CONSTANT_BUFFER_H
#define JOJ_D3D11_CONSTANT_BUFFER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>
#include "error_code.h"

namespace joj
{
    inline u32 calculate_cb_byte_size(u32 byte_size)
    {
        // Constant buffers must be a multiple of the minimum hardware
        // allocation size (usually 256 bytes).  So round up to nearest
        // multiple of 256.  We do this by adding 255 and then masking off
        // the lower 2 bytes which store all bits < 256.
        // Example: Suppose byte_size = 300.
        // (300 + 255) & ~255
        // 555 & ~255
        // 0x022B & ~0x00ff
        // 0x022B & 0xff00
        // 0x0200
        // 512
        return (byte_size + 255) & ~255;
    }

    class JAPI D3D11ConstantBuffer  
    {
    public:
        D3D11ConstantBuffer();
        ~D3D11ConstantBuffer();

        void setup(const u32 byte_width, const void* data);

        ErrorCode create(ID3D11Device* device);

        void bind_to_vertex_shader(ID3D11DeviceContext* device_context,
            u32 start_slot, u32 num_buffers);
        void bind_to_pixel_shader(ID3D11DeviceContext* device_context,
            u32 start_slot, u32 num_buffers);
        
        template<typename T>
        void update(ID3D11DeviceContext* device_context, T& data);

        const D3D11_BUFFER_DESC* get_buffer_desc() const;
        ID3D11Buffer*& get_buffer();
        const D3D11_SUBRESOURCE_DATA* get_subdata() const;

    private:
        ID3D11Buffer* m_buffer;
        D3D11_SUBRESOURCE_DATA m_init_data;
        D3D11_BUFFER_DESC m_cbd;
    };

    inline const D3D11_BUFFER_DESC* D3D11ConstantBuffer::get_buffer_desc() const
    { return &m_cbd; }

    inline ID3D11Buffer*& D3D11ConstantBuffer::get_buffer()
    { return m_buffer; }

    inline const D3D11_SUBRESOURCE_DATA* D3D11ConstantBuffer::get_subdata() const
    { return &m_init_data; }

    template<typename T>
    void D3D11ConstantBuffer::update(ID3D11DeviceContext* device_context, T& data)
    {
        // Data to be copied
        D3D11_MAPPED_SUBRESOURCE mapped_buffer = {};

        // Get a pointer to the constant buffer data and lock it
        device_context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);

        // Copy the new data to the constant buffer data.
        memcpy(mapped_buffer.pData, &data, sizeof(T));

        // Release the pointer to the constant buffer data.
        device_context->Unmap(m_buffer, 0);
    }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_CONSTANT_BUFFER_H