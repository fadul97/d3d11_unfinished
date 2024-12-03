#ifndef JOJ_D3D11_SPRITE_ANIMATION_H
#define JOJ_D3D11_SPRITE_ANIMATION_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <vector>
#include "animation_frame_d3d11.h"

namespace joj
{
    class JAPI D3D11SpriteAnimation
    {
    public:
        D3D11SpriteAnimation();
        D3D11SpriteAnimation(u32 frames_per_second, b8 loop);
        ~D3D11SpriteAnimation();

        void add_frame(D3D11AnimationFrame frame);

        void update(const f32 dt);

        const JFloat2 get_tex_coord() const;

    private:
        std::vector<D3D11AnimationFrame> m_frames;
        u32 m_current_frame;
        u32 m_total_frames;
        f32 m_delay;
        f32 m_time_elapsed;
        b8 m_loop;
    };

    inline const JFloat2 D3D11SpriteAnimation::get_tex_coord() const
    { return m_frames[m_current_frame].get_tex_coord(); }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_SPRITE_ANIMATION_H