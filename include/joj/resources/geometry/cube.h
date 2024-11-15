#ifndef JOJ_CUBE_H
#define JOJ_CUBE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "geometry.h"

namespace joj
{
    class JAPI Cube : public Geometry
    {
    public:
        Cube();
        Cube(f32 width, f32 height, f32 depth);
        Cube(f32 width, f32 height, f32 depth, const JFloat4 color);
    };
}

#endif // JOJ_CUBE_H