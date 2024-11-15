#ifndef JOJ_GEOMETRY_H
#define JOJ_GEOMETRY_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "math/jmath.h"
#include "geometry_vertex.h"
#include <vector>

namespace joj
{
    enum class GeometryType { UNKNOWN, CUBE, CYLINDER, SPHERE, GEOSPHERE, GRID, QUAD };

    class JAPI Geometry
    {
    public:
        Geometry();                                         // construtor
        virtual ~Geometry();                                // destrutor

        virtual void translate(f32 dx, f32 dy, f32 dz);
        virtual void move_to(f32 px, f32 py, f32 pz);

        virtual JFloat3 get_position() const;

        const GeometryVertex* get_vertex_data() const;
        const u32* get_index_data() const;

        u32 get_vertex_count() const;
        u32 get_index_count() const;

        std::vector<Vertex>& get_vertices();
        std::vector<u32>& get_indices();

        std::vector<u16>& get_indices16();    // TODO: Refactor

    protected:
        GeometryType m_type;

        JFloat3 m_position;

        std::vector<GeometryVertex> m_vertices;
        std::vector<u32> m_indices;
        std::vector<u16> m_indices16;    // TODO: Refactor

        void subdivide();
    };

    inline void Geometry::translate(const f32 dx, const f32 dy, const f32 dz)
    { m_position.x += dx; m_position.y += dy; m_position.z += dz; }

    inline void Geometry::move_to(const f32 px, const f32 py, const f32 pz)
    { m_position.x = px; m_position.y = py; m_position.z = pz; }

    inline DirectX::XMFLOAT3 Geometry::get_position() const
    { return m_position; }

    inline const Vertex* Geometry::get_vertex_data() const
    { return m_vertices.data(); }

    inline const u32* Geometry::get_index_data() const
    { return m_indices.data(); }

    inline u32 Geometry::get_vertex_count() const
    { return static_cast<u32>(m_vertices.size()); }

    inline u32 Geometry::get_index_count() const
    { return static_cast<u32>(m_indices.size()); }

    inline std::vector<Vertex>& Geometry::get_vertices()
    { return m_vertices; }

    inline std::vector<u32>& Geometry::get_indices()
    { return m_indices; }

    inline std::vector<u16>& Geometry::get_indices16()
    {
        if (m_indices16.empty())
        {
            m_indices16.resize(m_indices.size());
            for (size_t i = 0; i < m_indices.size(); ++i)
                m_indices16[i] = static_cast<u16>(m_indices[i]);
        }

        return m_indices16;
    }
}

#endif // JOJ_GEOMETRY_H