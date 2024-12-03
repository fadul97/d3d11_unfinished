#ifndef JOJ_D3D11_SPRITE_H
#define JOJ_D3D11_SPRITE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "error_code.h"
#include <d3d11.h>
#include "renderer/renderer.h"
#include <string>
#include <math/jmath.h>

namespace joj
{
    class JAPI D3D11Sprite
    {
    public:
        D3D11Sprite();
        ~D3D11Sprite();

        void bind(ID3D11DeviceContext* device_context, u32 start_slot, u32 num_views);
        void unbind(ID3D11DeviceContext* device_context);

        ErrorCode create(ID3D11Device* device,
            ID3D11DeviceContext* device_context,
            const std::wstring& filepath, ImageType type);
        void destroy();

        const JFloat2 get_tex_coord() const;
        const JFloat2 get_cell_size() const;

    private:
        ID3D11ShaderResourceView* m_srv;
        u32 m_width;
        u32 m_height;
    };

    inline const JFloat2 D3D11Sprite::get_tex_coord() const
    { return JFloat2{ 0.0f, 1.0f}; }

    inline const JFloat2 D3D11Sprite::get_cell_size() const
    { return JFloat2{ 1.0f, 1.0f}; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_SPRITE_H