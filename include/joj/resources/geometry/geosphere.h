#ifndef JOJ_GEOSPHERE_H
#define JOJ_GEOSPHERE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "geometry.h"

namespace joj
{
    class JAPI GeoSphere : public Geometry
    {
    public:
        GeoSphere();
        GeoSphere(f32 radius, u32 subdivisions);
        GeoSphere(f32 radius, u32 subdivisions, const JFloat4 color);
    };
}


#endif // JOJ_GEOSPHERE_H