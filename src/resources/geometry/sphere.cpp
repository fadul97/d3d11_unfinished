#include "resources/geometry/sphere.h"

joj::Sphere::Sphere()
{
    m_type = GeometryType::SPHERE;
}

joj::Sphere::Sphere(f32 radius, f32 slice_count, u32 layer_count)
{
    const JFloat4 color1(1.0f, 0.0f, 0.0f, 1.0f); // Red
    const JFloat4 color2(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

    GeometryVertex top_vertex;
    top_vertex.pos = DirectX::XMFLOAT3{ 0.0f, radius, 0.0f };
    top_vertex.normal = JFloat3(0.0f, 1.0f, 0.0f);
    top_vertex.color = color1;
    top_vertex.tex = JFloat2(0.0f, 0.0f);

    GeometryVertex bottom_vertex;
    bottom_vertex.pos = DirectX::XMFLOAT3{ 0.0f, -radius, 0.0f };
    bottom_vertex.normal = JFloat3(0.0f, -1.0f, 0.0f);
    bottom_vertex.color = color1;
    bottom_vertex.tex = JFloat2(0.0f, 1.0f);

    m_vertices.push_back(top_vertex);

    const f32 phi_step = J_PI / layer_count;
    const f32 theta_step = 2.0f * J_PI / slice_count;

    // Calculate the vertices for each ring (does not count the poles as rings)
    for (u32 i = 1; i <= layer_count - 1; ++i)
    {
        f32 phi = i * phi_step;

        // Ring vertices
        for (u32 j = 0; j <= slice_count; ++j)
        {
            f32 theta = j * theta_step;

            GeometryVertex v;

            // Spherical coordinates for Cartesian coordinates
            v.pos.x = radius * sinf(phi) * cosf(theta);
            v.pos.y = radius * cosf(phi);
            v.pos.z = radius * sinf(phi) * sinf(theta);

            // Normal 
            JVector4 p = DirectX::XMLoadFloat3(&v.pos);
            DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

            if (i % 2 == 0)
                v.color = color1;
            else
                v.color = color2;

            v.tex.x = theta / J_PI;
            v.tex.y = phi / J_PI;

            m_vertices.push_back(v);
        }
    }

    m_vertices.push_back(bottom_vertex);

    // Calculate the indexes of the top layer
    // This layer connects the top pole to the first ring
    for (u32 i = 1; i <= slice_count; ++i)
    {
        m_indices.push_back(0);
        m_indices.push_back(i + 1);
        m_indices.push_back(i);
    }

    // Calculate the indexes for the inner layers (not connected to the poles)
    u32 base_index = 1;
    const u32 ring_vertex_count = slice_count + 1;
    for (u32 i = 0; i < layer_count - 2; ++i)
    {
        for (u32 j = 0; j < slice_count; ++j)
        {
            m_indices.push_back(base_index + i * ring_vertex_count + j);
            m_indices.push_back(base_index + i * ring_vertex_count + j + 1);
            m_indices.push_back(base_index + (i + 1) * ring_vertex_count + j);

            m_indices.push_back(base_index + (i + 1) * ring_vertex_count + j);
            m_indices.push_back(base_index + i * ring_vertex_count + j + 1);
            m_indices.push_back(base_index + (i + 1) * ring_vertex_count + j + 1);
        }
    }

    // Bottom pole is added last
    const u32 south_pole_index = (u32)m_vertices.size() - 1;

    // It's positioned at the indices of the first vertex of the last ring
    base_index = south_pole_index - ring_vertex_count;

    for (u32 i = 0; i < slice_count; ++i)
    {
        m_indices.push_back(south_pole_index);
        m_indices.push_back(base_index + i);
        m_indices.push_back(base_index + i + 1);
    }
}

joj::Sphere::Sphere(f32 radius, f32 slice_count, u32 layer_count, const JFloat4 color)
{
    GeometryVertex top_vertex;
    top_vertex.pos = DirectX::XMFLOAT3{ 0.0f, radius, 0.0f };
    top_vertex.color = color;
    top_vertex.tex = JFloat2(0.0f, 0.0f);

    GeometryVertex bottom_vertex;
    bottom_vertex.pos = DirectX::XMFLOAT3{ 0.0f, -radius, 0.0f };
    bottom_vertex.color = color;
    bottom_vertex.tex = JFloat2(0.0f, 1.0f);

    m_vertices.push_back(top_vertex);

    const f32 phi_step = DirectX::XM_PI / layer_count;
    const f32 theta_step = 2.0f * DirectX::XM_PI / slice_count;

    // Calculate the vertices for each ring (does not count the poles as rings)
    for (u32 i = 1; i <= layer_count - 1; ++i)
    {
        f32 phi = i * phi_step;

        // Ring vertices
        for (u32 j = 0; j <= slice_count; ++j)
        {
            f32 theta = j * theta_step;

            GeometryVertex v;

            // Spherical coordinates for Cartesian coordinates
            v.pos.x = radius * sinf(phi) * cosf(theta);
            v.pos.y = radius * cosf(phi);
            v.pos.z = radius * sinf(phi) * sinf(theta);

            // Normal 
            JVector4 p = DirectX::XMLoadFloat3(&v.pos);
            DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

            v.color = color;

            v.tex.x = theta / J_PI;
            v.tex.y = phi / J_PI;

            m_vertices.push_back(v);
        }
    }

    m_vertices.push_back(bottom_vertex);

    // Calculate the indexes of the top layer
    // This layer connects the top pole to the first ring
    for (u32 i = 1; i <= slice_count; ++i)
    {
        m_indices.push_back(0);
        m_indices.push_back(i + 1);
        m_indices.push_back(i);
    }

    // Calculate the indexes for the inner layers (not connected to the poles)
    u32 base_index = 1;
    const u32 ring_vertex_count = slice_count + 1;
    for (u32 i = 0; i < layer_count - 2; ++i)
    {
        for (u32 j = 0; j < slice_count; ++j)
        {
            m_indices.push_back(base_index + i * ring_vertex_count + j);
            m_indices.push_back(base_index + i * ring_vertex_count + j + 1);
            m_indices.push_back(base_index + (i + 1) * ring_vertex_count + j);

            m_indices.push_back(base_index + (i + 1) * ring_vertex_count + j);
            m_indices.push_back(base_index + i * ring_vertex_count + j + 1);
            m_indices.push_back(base_index + (i + 1) * ring_vertex_count + j + 1);
        }
    }

    // Bottom pole is added last
    const u32 south_pole_index = (u32)m_vertices.size() - 1;

    // It's positioned at the indices of the first vertex of the last ring
    base_index = south_pole_index - ring_vertex_count;

    for (u32 i = 0; i < slice_count; ++i)
    {
        m_indices.push_back(south_pole_index);
        m_indices.push_back(base_index + i);
        m_indices.push_back(base_index + i + 1);
    }
}