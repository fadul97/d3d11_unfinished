#ifndef JOJ_SPRITE_ANIMATION_H
#define JOJ_SPRITE_ANIMATION_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "math/jmath.h"

namespace joj
{
    class JAPI D3D11SpriteAnimation
    {
    public:
        D3D11SpriteAnimation();
        D3D11SpriteAnimation(i32 total_frames, i32 columns, i32 rows, u32 frames_per_second);
        ~D3D11SpriteAnimation();

        void update(const f32 dt);

        const JFloat2& get_uv_offset() const;
        const JFloat2& get_cell_size() const;

    private:
        i32 m_total_frames;
        i32 m_columns;
        i32 m_rows;
        f32 m_delay;
        f32 m_time_elapsed;
        i32 m_current_frame;
        JFloat2 m_uv_offset;
        JFloat2 m_cell_size;
    };

    inline const JFloat2& D3D11SpriteAnimation::get_uv_offset() const
    { return m_uv_offset; }

    inline const JFloat2& D3D11SpriteAnimation::get_cell_size() const
    { return m_cell_size; }
}

#endif // JOJ_SPRITE_ANIMATION_H