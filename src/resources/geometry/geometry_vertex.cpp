#include "resources/geometry/geometry_vertex.h"

joj::GeometryVertex::GeometryVertex()
    : pos(0, 0, 0), normal(0, 0, 0), color(0, 0, 0, 0), tex(0, 0)
{
}

joj::GeometryVertex::GeometryVertex(f32 px, f32 py, f32 pz, f32 nx, f32 ny, f32 nz, f32 r, f32 g, f32 b, f32 a, f32 u, f32 v)
    : pos(px, py, pz), normal(nx, ny, nz), color(r, g, b, a), tex(u, v)
{
}

joj::GeometryVertex::GeometryVertex(JFloat3 position, JFloat3 normal, JFloat4 color, JFloat2 texture)
    : pos(position), normal(normal), color(color), tex(texture)
{
}