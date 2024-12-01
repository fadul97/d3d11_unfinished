#ifndef JOJ_D3D11_INDEX_BUFFER_H
#define JOJ_D3D11_INDEX_BUFFER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>
#include "error_code.h"

namespace joj
{
    class JAPI D3D11IndexBuffer
    {
    public:
        D3D11IndexBuffer();
        ~D3D11IndexBuffer();

        void setup(u32 byte_width, const void* data);

        ErrorCode create(ID3D11Device* device);

        void bind(ID3D11DeviceContext* device_context,
            DXGI_FORMAT format, u32 offset);

        const D3D11_BUFFER_DESC* get_buffer_desc() const;
        ID3D11Buffer*& get_buffer();
        const D3D11_SUBRESOURCE_DATA* get_subdata() const;

    private:
        D3D11_BUFFER_DESC m_ibd;
        ID3D11Buffer* m_index_buffer;
        D3D11_SUBRESOURCE_DATA m_init_data;
    };

    inline const D3D11_BUFFER_DESC* D3D11IndexBuffer::get_buffer_desc() const
    { return &m_ibd; }

    inline ID3D11Buffer*& D3D11IndexBuffer::get_buffer()
    { return m_index_buffer; }

    inline const D3D11_SUBRESOURCE_DATA* D3D11IndexBuffer::get_subdata() const
    { return &m_init_data; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_INDEX_BUFFER_H