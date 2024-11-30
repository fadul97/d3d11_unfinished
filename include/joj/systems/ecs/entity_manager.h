#ifndef JOJ_ENTITY_MANAGER_H
#define JOJ_ENTITY_MANAGER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "entity.h"

namespace joj
{
    class JAPI EntityManager
    {
    public:
        EntityManager();
        ~EntityManager();

        Entity create_entity();
    private:
        Entity m_next_id;
    };
}

#endif // JOJ_ENTITY_MANAGER_H