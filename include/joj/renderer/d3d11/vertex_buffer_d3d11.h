#ifndef JOJ_D3D11_VERTEX_BUFFER_H
#define JOJ_D3D11_VERTEX_BUFFER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>
#include "error_code.h"

namespace joj
{
    class JAPI D3D11VertexBuffer
    {
    public:
        D3D11VertexBuffer();
        ~D3D11VertexBuffer();

        void setup(D3D11_USAGE usage, u32 cpu_access_flags, u32 byte_width, const void* data);

        ErrorCode create(ID3D11Device* device);

        void bind(ID3D11DeviceContext* device_context,
            u32 start_slot, u32 num_buffers, const u32* stride, const u32* offset);

        const D3D11_BUFFER_DESC* get_buffer_desc() const;
        ID3D11Buffer*& get_buffer();
        const D3D11_SUBRESOURCE_DATA* get_subdata() const;

        b8 is_filled() const;
        void cleanup();

    private:
        D3D11_BUFFER_DESC m_vbd;
        ID3D11Buffer* m_vertex_buffer;
        D3D11_SUBRESOURCE_DATA m_init_data;

        b8 m_filled;
    };

    inline const D3D11_BUFFER_DESC* D3D11VertexBuffer::get_buffer_desc() const
    { return &m_vbd; }

    inline ID3D11Buffer*& D3D11VertexBuffer::get_buffer()
    { return m_vertex_buffer; }

    inline const D3D11_SUBRESOURCE_DATA* D3D11VertexBuffer::get_subdata() const
    { return &m_init_data; }

    inline b8 D3D11VertexBuffer::is_filled() const
    { return m_filled; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_VERTEX_BUFFER_H