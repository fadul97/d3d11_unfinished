#ifndef JOJ_D3D11_TILESET_H
#define JOJ_D3D11_TILESET_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>
#include "error_code.h"
#include <string>

namespace joj
{
    enum class ImageType
    {
        PNG,
        DDS
    };

    class JAPI D3D11Tileset
    {
    public:
        D3D11Tileset();
        ~D3D11Tileset();

        ErrorCode create(ID3D11Device* device,
            ID3D11DeviceContext* device_context,
            const std::wstring& filepath, ImageType type);
        void destroy();

    private:
        u32 m_width;
        u32 m_height;
        ID3D11ShaderResourceView* m_srv;
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_TILESET_H