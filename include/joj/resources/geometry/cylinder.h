#ifndef JOJ_CYLINDER_H
#define JOJ_CYLINDER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "geometry.h"

namespace joj
{
    class JAPI Cylinder : public Geometry
    {
    public:
        Cylinder();
        Cylinder(f32 bottom_radius, f32 top_radius, f32 height, u32 slide_count, u32 layer_count);
        Cylinder(f32 bottom_radius, f32 top_radius, f32 height, u32 slide_count, u32 layer_count, const JFloat4 color);

    private:
        void build_top_cap(f32 bottom_radius, f32 top_radius, f32 height, u32 slice_count, u32 stack_count, const JFloat4 color);
        void build_bottom_cap(f32 bottom_radius, f32 top_radius, f32 height, u32 slice_count, u32 stack_count, const JFloat4 color);
    };
}

#endif // JOJ_CYLINDER_H