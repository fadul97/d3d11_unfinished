#ifndef JOJ_D3D11_ANIMATION_FRAME_H
#define JOJ_D3D11_ANIMATION_FRAME_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "sprite_sheet_d3d11.h"
#include "math/jmath.h"

namespace joj
{
    class JAPI D3D11AnimationFrame
    {
    public:
        D3D11AnimationFrame();
        D3D11AnimationFrame(D3D11SpriteSheet& sprite_sheet, u32 x, u32 y);
        ~D3D11AnimationFrame();

        const JFloat2 get_tex_coord() const;
        const JFloat2 get_cell_size() const;

    private:
        D3D11SpriteSheet* m_sprite_sheet;
        f32 m_x0;
        f32 m_y0;
        f32 m_xsize;
        f32 m_ysize;
    };

    inline const JFloat2 D3D11AnimationFrame::get_tex_coord() const
    { return JFloat2{ m_x0, m_y0 }; }

    inline const JFloat2 D3D11AnimationFrame::get_cell_size() const
    { return JFloat2{ m_xsize, m_ysize }; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_ANIMATION_FRAME_H