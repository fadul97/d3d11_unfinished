#ifndef JOJ_D3D11_SPRITE_SHEET_H
#define JOJ_D3D11_SPRITE_SHEET_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "error_code.h"
#include <d3d11.h>

namespace joj
{
    class JAPI D3D11SpriteSheet
    {
    public:
        D3D11SpriteSheet();
        ~D3D11SpriteSheet();

        ErrorCode create(ID3D11Device* device, const wchar_t* filepath,
            u32 width, u32 height, u32 rows, u32 columns);
        void destroy();

        void bind(ID3D11DeviceContext* device_context, u32 start_slot, u32 num_views);
        void unbind(ID3D11DeviceContext* device_context);

        u32 get_width() const;
        u32 get_height() const;

        u32 get_cell_width() const;
        u32 get_cell_height() const;

        u32 get_rows() const;
        u32 get_columns() const;
        
    private:
        ID3D11ShaderResourceView* m_srv;
        u32 m_width;
        u32 m_height;
        u32 m_cell_width;
        u32 m_cell_height;
        u32 m_rows;
        u32 m_columns;
    };

    inline u32 D3D11SpriteSheet::get_width() const
    { return m_width; }

    inline u32 D3D11SpriteSheet::get_height() const
    { return m_height; }

    inline u32 D3D11SpriteSheet::get_cell_width() const
    { return m_cell_width; }

    inline u32 D3D11SpriteSheet::get_cell_height() const
    { return m_cell_height; }

    inline u32 D3D11SpriteSheet::get_rows() const
    { return m_rows; }

    inline u32 D3D11SpriteSheet::get_columns() const
    { return m_columns; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_SPRITE_SHEET_H