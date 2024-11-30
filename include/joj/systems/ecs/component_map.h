#ifndef JOJ_COMPONENTS_MAP_H
#define JOJ_COMPONENTS_MAP_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include <unordered_map>
#include "entity.h"
#include "components.h"

namespace joj
{
    template <typename T>
    class JAPI ComponentMap
    {
    public:
        ComponentMap();
        ~ComponentMap();

        void add(Entity entity, const T& component);
        void remove(Entity entity);

        T* get(Entity entity);
        const std::unordered_map<Entity, T>& get_all() const;

    private:
        std::unordered_map<Entity, T> m_components;
    };

    template <typename T>
    ComponentMap<T>::ComponentMap()
    {

    }

    template <typename T>
    ComponentMap<T>::~ComponentMap()
    {

    }

    template <typename T>
    inline void ComponentMap<T>::add(Entity entity, const T& component)
    {
        m_components[entity] = component;
    }

    template <typename T>
    inline void ComponentMap<T>::remove(Entity entity)
    {
        m_components.erase(entity);
    }

    template <typename T>
    inline T* ComponentMap<T>::get(Entity entity)
    {
        if (m_components.find(entity) != m_components.end())
            return &m_components[entity];

        return nullptr;
    }

    template <typename T>
    inline const std::unordered_map<Entity, T>& ComponentMap<T>::get_all() const {
        return m_components;
    }
}

#endif // JOJ_COMPONENTS_MAP_H