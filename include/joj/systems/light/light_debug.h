#ifndef JOJ_LIGHT_DEBUG_H
#define JOJ_LIGHT_DEBUG_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "light.h"

namespace joj
{
    b8 are_directional_lights_equals(DirectionalLight& l1, DirectionalLight& l2);
    b8 are_point_lights_equals(PointLight& l1, PointLight& l2);
    b8 are_spot_lights_equals(SpotLight& l1, SpotLight& l2);
}

#endif // JOJ_LIGHT_DEBUG_H