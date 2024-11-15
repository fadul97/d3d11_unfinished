#ifndef JOJ_GRID_H
#define JOJ_GRID_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "geometry.h"

namespace joj
{
    class JAPI Grid : public Geometry
    {
    public:
        Grid();
        // m = lines, n = columns
        Grid(f32 width, f32 depth, u32 m, u32 n);
        Grid(f32 width, f32 depth, u32 m, u32 n, const JFloat4 color);
    };
}


#endif // JOJ_GRID_H