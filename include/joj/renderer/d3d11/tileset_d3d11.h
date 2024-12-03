#ifndef JOJ_D3D11_TILESET_H
#define JOJ_D3D11_TILESET_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>
#include "error_code.h"
#include <string>
#include "renderer/renderer.h"

namespace joj
{
    class JAPI D3D11Tileset
    {
    public:
        D3D11Tileset();
        ~D3D11Tileset();

        ErrorCode create(ID3D11Device* device,
            ID3D11DeviceContext* device_context,
            const std::wstring& filepath, ImageType type,
            u32 tile_width, u32 tile_height,
            u32 columns, u32 num_tiles);
        void destroy();

        u32 get_width() const;
        u32 get_height() const;

        u32 get_tile_width() const;
        u32 get_tile_height() const;

        u32 get_columns() const;
        u32 get_size() const;
        ID3D11ShaderResourceView* m_srv;

    private:
        u32 m_width;
        u32 m_height;
        u32 m_tile_width;
        u32 m_tile_height;
        u32 m_columns;
        u32 m_size;
    };

    inline u32 D3D11Tileset::get_width() const
    { return m_width; }

    inline u32 D3D11Tileset::get_height() const
    { return m_height; }

    inline u32 D3D11Tileset::get_tile_width() const
    { return m_tile_width; }

    inline u32 D3D11Tileset::get_tile_height() const
    { return m_tile_height; }

    inline u32 D3D11Tileset::get_size() const
    { return m_size; }

    inline u32 D3D11Tileset::get_columns() const
    { return m_columns; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_TILESET_H