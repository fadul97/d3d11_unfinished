#ifndef JOJ_MOVEMENT_SYSTEM_H
#define JOJ_MOVEMENT_SYSTEM_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "entity.h"
#include "component_map.h"
#include "components.h"

namespace joj
{
    namespace MovementSystem
    {
        JAPI void update(ComponentMap<Position2D> positions, ComponentMap<Velocity2D> velocities, const f32 dt);
    };
}

#endif // JOJ_MOVEMENT_SYSTEM_H