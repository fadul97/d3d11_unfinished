#include "resources/geometry/geometry.h"

#if JPLATFORM_WINDOWS

joj::Geometry::Geometry()
    : m_type(GeometryType::UNKNOWN)
{
}

joj::Geometry::~Geometry()
{
}

void joj::Geometry::subdivide()
{
    using namespace DirectX; // For math operation in loop

    // Save a copy of the original geometry
    std::vector <GeometryVertex> vertices_copy = m_vertices;
    std::vector <u32> indices_copy = m_indices;

    m_vertices.resize(0);
    m_indices.resize(0);

    //       v1
    //       *
    //      / \
    //     /   \
    //  m0*-----*m1
    //   / \   / \
    //  /   \ /   \
    // *-----*-----*
    // v0    m2     v2

    const u32 num_tris = static_cast<u32>(indices_copy.size() / 3);

    for (u32 i = 0; i < num_tris; ++i)
    {
        GeometryVertex v0 = vertices_copy[indices_copy[size_t(i) * 3 + 0]];
        GeometryVertex v1 = vertices_copy[indices_copy[size_t(i) * 3 + 1]];
        GeometryVertex v2 = vertices_copy[indices_copy[size_t(i) * 3 + 2]];

        // Find center points of each edge
        GeometryVertex m0, m1, m2;
        DirectX::XMStoreFloat3(&m0.pos, 0.5f * (DirectX::XMLoadFloat3(&v0.pos) + DirectX::XMLoadFloat3(&v1.pos)));
        DirectX::XMStoreFloat3(&m1.pos, 0.5f * (DirectX::XMLoadFloat3(&v1.pos) + DirectX::XMLoadFloat3(&v2.pos)));
        DirectX::XMStoreFloat3(&m2.pos, 0.5f * (DirectX::XMLoadFloat3(&v0.pos) + DirectX::XMLoadFloat3(&v2.pos)));

        // Add new geometry
        m_vertices.push_back(v0); // 0
        m_vertices.push_back(v1); // 1
        m_vertices.push_back(v2); // 2
        m_vertices.push_back(m0); // 3
        m_vertices.push_back(m1); // 4
        m_vertices.push_back(m2); // 5

        m_indices.push_back(i * 6 + 0);
        m_indices.push_back(i * 6 + 3);
        m_indices.push_back(i * 6 + 5);

        m_indices.push_back(i * 6 + 3);
        m_indices.push_back(i * 6 + 4);
        m_indices.push_back(i * 6 + 5);

        m_indices.push_back(i * 6 + 5);
        m_indices.push_back(i * 6 + 4);
        m_indices.push_back(i * 6 + 2);

        m_indices.push_back(i * 6 + 3);
        m_indices.push_back(i * 6 + 1);
        m_indices.push_back(i * 6 + 4);
    }
}

#endif // JPLATFORM_WINDOWS