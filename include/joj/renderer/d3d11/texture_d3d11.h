#ifndef JOJ_D3D11_TEXTURE_H
#define JOJ_D3D11_TEXTURE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "error_code.h"
#include <d3d11.h>

namespace joj
{
    class JAPI D3D11Texture
    {
    public:
        D3D11Texture();
        D3D11Texture(const wchar_t* filepath);
        ~D3D11Texture();

        void bind(u32 start_slot, u32 num_views);
        void unbind();

        ErrorCode create(const wchar_t* filepath);
        void destroy();

    private:
        ID3D11ShaderResourceView* m_srv;
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_TEXTURE_H