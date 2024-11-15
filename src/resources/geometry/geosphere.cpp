#include "resources/geometry/geosphere.h"

joj::GeoSphere::GeoSphere()
{
    m_type = GeometryType::GEOSPHERE;
}

joj::GeoSphere::GeoSphere(f32 radius, u32 subdivisions)
{
    using namespace DirectX;

    m_type = GeometryType::GEOSPHERE;

    const JFloat4 color1(1.0f, 0.0f, 0.0f, 1.0f); // Red
    const JFloat4 color2(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

    // Max number of subdivisions is 6
    subdivisions = (subdivisions > 6U ? 6U : subdivisions);

    // Approximate a sphere by the subdivision of an icosahedron
    const f32 X = 0.525731f;
    const f32 Z = 0.850651f;

    // Vertices of the icosahedron
    const JFloat3 pos[12] =
    {
        JFloat3{-X, 0.0f, Z},  JFloat3{X, 0.0f, Z},
        JFloat3{-X, 0.0f, -Z}, JFloat3{X, 0.0f, -Z},
        JFloat3{0.0f, Z, X},   JFloat3{0.0f, Z, -X},
        JFloat3{0.0f, -Z, X},  JFloat3{0.0f, -Z, -X},
        JFloat3{Z, X, 0.0f},   JFloat3{-Z, X, 0.0f},
        JFloat3{Z, -X, 0.0f},  JFloat3{-Z, -X, 0.0f}
    };

    // Indices of the icosahedron
    const u32 k[60] =
    {
        1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
        1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
        3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
        10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
    };

    // Snap and initialize vertex vectors and indices
    m_vertices.resize(12);
    m_indices.assign(&k[0], &k[60]);

    for (u32 i = 0; i < 12; ++i)
        m_vertices[i].pos = pos[i];

    // Subdivide each triangle of the icosahedron a certain number of times
    for (u32 i = 0; i < subdivisions; ++i)
        subdivide();

    // Project the vertices onto a sphere and adjust the scale
    for (u32 i = 0; i < m_vertices.size(); ++i)
    {
        // Normalize vetor (point)
        const JVector4 n = DirectX::XMVector3Normalize(XMLoadFloat3(&m_vertices[i].pos));

        // Project on sphere
        const JVector4 p = radius * n;

        DirectX::XMStoreFloat3(&m_vertices[i].pos, p);
        DirectX::XMStoreFloat3(&m_vertices[i].normal, n);

        // Derive texture coordinates from spherical coordinates
        f32 theta = atan2f(m_vertices[i].pos.z, m_vertices[i].pos.x);

        // Put in [0, 2pi].
        if (theta < 0.0f)
            theta += J_2PI;

        const f32 phi = acosf(m_vertices[i].pos.y / radius);

        m_vertices[i].tex.x = theta / J_2PI;
        m_vertices[i].tex.y = phi / J_PI;

        if (i % 2 == 0)
            m_vertices[i].color = color1;
        else
            m_vertices[i].color = color2;
    }
}

joj::GeoSphere::GeoSphere(f32 radius, u32 subdivisions, const JFloat4 color)
{
    using namespace DirectX;

    m_type = GeometryType::GEOSPHERE;

    // Max number of subdivisions is 6
    subdivisions = (subdivisions > 6U ? 6U : subdivisions);

    // Approximate a sphere by the subdivision of an icosahedron
    const f32 X = 0.525731f;
    const f32 Z = 0.850651f;

    // Vertices of the icosahedron
    const JFloat3 pos[12] =
    {
        JFloat3{-X, 0.0f, Z},  JFloat3{X, 0.0f, Z},
        JFloat3{-X, 0.0f, -Z}, JFloat3{X, 0.0f, -Z},
        JFloat3{0.0f, Z, X},   JFloat3{0.0f, Z, -X},
        JFloat3{0.0f, -Z, X},  JFloat3{0.0f, -Z, -X},
        JFloat3{Z, X, 0.0f},   JFloat3{-Z, X, 0.0f},
        JFloat3{Z, -X, 0.0f},  JFloat3{-Z, -X, 0.0f}
    };

    // Indices of the icosahedron
    const u32 k[60] =
    {
        1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
        1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
        3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
        10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
    };

    // Snap and initialize vertex vectors and indices
    m_vertices.resize(12);
    m_indices.assign(&k[0], &k[60]);

    for (u32 i = 0; i < 12; ++i)
        m_vertices[i].pos = pos[i];

    // Subdivide each triangle of the icosahedron a certain number of times
    for (u32 i = 0; i < subdivisions; ++i)
        subdivide();

    // Project the vertices onto a sphere and adjust the scale
    for (u32 i = 0; i < m_vertices.size(); ++i)
    {
        // Normalize vetor (point)
        const JVector4 n = DirectX::XMVector3Normalize(XMLoadFloat3(&m_vertices[i].pos));

        // Project on sphere
        const JVector4 p = radius * n;

        DirectX::XMStoreFloat3(&m_vertices[i].pos, p);
        DirectX::XMStoreFloat3(&m_vertices[i].normal, n);

        // Derive texture coordinates from spherical coordinates
        f32 theta = atan2f(m_vertices[i].pos.z, m_vertices[i].pos.x);

        // Put in [0, 2pi].
        if (theta < 0.0f)
            theta += J_2PI;

        const f32 phi = acosf(m_vertices[i].pos.y / radius);

        m_vertices[i].tex.x = theta / J_2PI;
        m_vertices[i].tex.y = phi / J_PI;

        m_vertices[i].color = color;
    }
}