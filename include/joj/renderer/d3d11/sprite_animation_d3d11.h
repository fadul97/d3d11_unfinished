#ifndef JOJ_D3D11_SPRITE_ANIMATION_H
#define JOJ_D3D11_SPRITE_ANIMATION_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <unordered_map>
#include "animation_frame_d3d11.h"

namespace joj
{
    // Vector and size
    using D3D11AnimationSequence = std::pair<D3D11AnimationFrame*, u32>;

    class JAPI D3D11SpriteAnimation
    {
    public:
        D3D11SpriteAnimation();
        D3D11SpriteAnimation(u32 frames_per_second, b8 loop);
        ~D3D11SpriteAnimation();

        void add_frame(u32 id, D3D11AnimationFrame* frames, u32 size);

        void select(u32 id);

        void update(const f32 dt);

        const JFloat2 get_tex_coord() const;
        const JFloat2 get_cell_size() const;

    private:
        // key = id
        std::unordered_map<u32, D3D11AnimationSequence> m_frames;
        D3D11AnimationSequence* m_current_sequence;
        u32 m_current_id;
        u32 m_current_frame;
        u32 m_end_frame;
        f32 m_delay;
        f32 m_time_elapsed;
        b8 m_loop;
    };

    inline const JFloat2 D3D11SpriteAnimation::get_tex_coord() const
    { return m_frames.at(m_current_id).first[m_current_frame].get_tex_coord(); }

    inline const JFloat2 D3D11SpriteAnimation::get_cell_size() const
    { return m_frames.at(m_current_id).first[m_current_frame].get_cell_size(); }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_SPRITE_ANIMATION_H