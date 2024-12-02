#ifndef JOJ_SPRITE_ANIMATION_H
#define JOJ_SPRITE_ANIMATION_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "math/jmath.h"

namespace joj
{
    class JAPI SpriteAnimation
    {
    public:
        SpriteAnimation();
        SpriteAnimation(i32 rows, i32 columns, i32 start_frame, i32 end_frame, f32 delay, b8 loop);
        ~SpriteAnimation();

        void update(const f32 dt);
        void next_frame();
        void calculate_tex_coord();

        const JFloat2& get_tex_coord() const;
        const JFloat2& get_tex_size() const;

    private:
        i32 m_columns;
        i32 m_rows;
        f32 m_delay;
        b8 m_loop;
        i32 m_current_frame;
        i32 m_start_frame;
        i32 m_end_frame;
        f32 m_time_elapsed;
        JFloat2 m_tex_coord;
        JFloat2 m_tex_size;
    };

    inline const JFloat2& SpriteAnimation::get_tex_coord() const
    { return m_tex_coord; }

    inline const JFloat2& SpriteAnimation::get_tex_size() const
    { return m_tex_size; }
}

#endif // JOJ_SPRITE_ANIMATION_H