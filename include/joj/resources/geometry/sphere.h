#ifndef JOJ_SPHERE_H
#define JOJ_SPHERE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "geometry.h"

namespace joj
{
    class JAPI Sphere : public Geometry
    {
    public:
        Sphere();
        Sphere(f32 radius, f32 slice_count, u32 layer_count);
        Sphere(f32 radius, f32 slice_count, u32 layer_count, const JFloat4 color);
    };
}

#endif // JOJ_SPHERE_H