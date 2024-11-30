#include "systems/ecs/movement_system.h"

#include "systems/ecs/entity.h"

void joj::MovementSystem::update(ComponentMap<joj::Position2D> positions, ComponentMap<joj::Velocity2D> velocities, const f32 dt)
{
    for (const auto& pair : positions.get_all())
    {
        const auto& entity = pair.first;
        const auto& position = pair.second;

        if (auto velocity = velocities.get(entity))
        {
            positions.get(entity)->x += velocity->x * dt;
            positions.get(entity)->y += velocity->y * dt;
        }
    }
}