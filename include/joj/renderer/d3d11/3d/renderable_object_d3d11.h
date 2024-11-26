#ifndef JOJ_D3D11_RENDERABLE_OBJECT_H
#define JOJ_D3D11_RENDERABLE_OBJECT_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "math/jmath.h"

namespace joj
{
    // Lightweight class that stores parameters to draw a shape.
    // This will vary from app-to-app.
    class JAPI D3D11RenderableObject
    {
    public:
        D3D11RenderableObject();
        ~D3D11RenderableObject();

        JFloat4x4& get_world_float4x4();
        u32 get_index_count() const;
        u32 get_index_location() const;
        i32 get_vertex_location() const;

        void set_world_float4x4(const JFloat4x4 world);
        void set_index_count(const u32 index_count);
        void set_index_location(const u32 index_location);
        void set_vertex_location(const u32 vertex_location);

    private:
        // Object Matrix
        JFloat4x4 m_world;

        u32 m_index_count;
        u32 m_index_location;
        i32 m_vertex_location;
    };

    inline JFloat4x4& D3D11RenderableObject::get_world_float4x4()
    { return m_world; }

    inline u32 D3D11RenderableObject::get_index_count() const
    { return m_index_count; }

    inline u32 D3D11RenderableObject::get_index_location() const
    { return m_index_location; }

    inline i32 D3D11RenderableObject::get_vertex_location() const
    { return m_vertex_location; }

    inline void D3D11RenderableObject::set_world_float4x4(const JFloat4x4 world)
    { m_world = world; }

    inline void D3D11RenderableObject::set_index_count(const u32 index_count)
    { m_index_count = index_count; }

    inline void D3D11RenderableObject::set_index_location(const u32 index_location)
    { m_index_location = index_location; }

    inline void D3D11RenderableObject::set_vertex_location(const u32 vertex_location)
    { m_vertex_location = vertex_location; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_RENDERABLE_OBJECT_H