#include "systems/ecs/entity_manager.h"

joj::EntityManager::EntityManager()
    : m_next_id(0)
{
}

joj::EntityManager::~EntityManager()
{
}

joj::Entity joj::EntityManager::create_entity()
{
    return m_next_id++;
}