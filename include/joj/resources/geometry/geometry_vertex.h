#ifndef JOJ_GEOMETRY_VERTEX_H
#define JOJ_GEOMETRY_VERTEX_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <math/jmath.h>

namespace joj
{
    struct GeometryVertex
    {
        GeometryVertex();
        GeometryVertex(f32 px, f32 py, f32 pz, f32 nx, f32 ny, f32 nz, f32 r, f32 g, f32 b, f32 a, f32 u, f32 v);
        GeometryVertex(JFloat3 position, JFloat3 normal, JFloat4 color, JFloat2 texture);

        JFloat3 pos;
        JFloat3 normal;
        JFloat4 color;
        JFloat2 tex;
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_GEOMETRY_VERTEX_H