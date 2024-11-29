#ifndef JOJ_OBJECT_2D_H
#define JOJ_OBJECT_2D_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "math/jmath.h"

namespace joj
{
    class JAPI Object2D
    {
    public:
        Object2D();
        Object2D(f32 xpos, f32 ypos, f32 xsize, f32 ysize);
        ~Object2D();

        f32 get_xpos() const;
        f32 get_ypos() const;

        f32 get_xsize() const;
        f32 get_ysize() const;

        void move_to(const f32 x, const f32 y);

        void translate_xpos(const f32 dx);
        void translate_ypos(const f32 dy);
    
    private:
        JFloat2 m_pos;
        JFloat2 m_size;
    };

    inline f32 Object2D::get_xpos() const
    { return m_pos.x; }

    inline f32 Object2D::get_ypos() const
    { return m_pos.y; }

    inline f32 Object2D::get_xsize() const
    { return m_size.x; }

    inline f32 Object2D::get_ysize() const
    { return m_size.y; }

    inline void Object2D::translate_xpos(const f32 dx)
    { m_pos.x += dx; }

    inline void Object2D::translate_ypos(const f32 dy)
    { m_pos.y += dy; }

    inline void Object2D::move_to(const f32 x, const f32 y)
    { m_pos.x = x; m_pos.y = y; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_OBJECT_2D_H