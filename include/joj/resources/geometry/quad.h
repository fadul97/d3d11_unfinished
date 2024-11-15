#ifndef JOJ_QUAD_H
#define JOJ_QUAD_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "geometry.h"

namespace joj
{
    class JAPI Quad : public Geometry
    {
    public:
        Quad();
        Quad(f32 width, f32 height);
        Quad(f32 width, f32 height, const JFloat4 color);
    };
}

#endif // JOJ_QUAD_H