#include "resources/geometry/cylinder.h"

joj::Cylinder::Cylinder()
{
    m_type = GeometryType::CYLINDER;
}

joj::Cylinder::Cylinder(f32 bottom, f32 top, f32 height, u32 slice_count, u32 layer_count)
{
    m_type = GeometryType::CYLINDER;

    const JFloat4 color1(1.0f, 0.0f, 0.0f, 1.0f);  // Red
    const JFloat4 color2(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow

    // Layer height
    const f32 layer_height = height / layer_count;

    // Increment in the radius of each layer
    const f32 radius_step = (top - bottom) / layer_count;

    // Number of cylinder rings
    const u32 ring_count = layer_count + 1;

    // Calculate vertices of each ring
    for (u32 i = 0; i < ring_count; ++i)
    {
        const f32 y = -0.5f * height + i * layer_height;
        const f32 r = bottom + i * radius_step;
        const f32 theta = 2.0f * DirectX::XM_PI / slice_count;

        for (u32 j = 0; j <= slice_count; ++j)
        {
            const f32 c = cosf(j * theta);
            const f32 s = sinf(j * theta);

            GeometryVertex vertex;
            vertex.pos = DirectX::XMFLOAT3{ r * c, y, r * s };

            vertex.tex.x = (f32)j / slice_count;
            vertex.tex.y = 1.0f - (f32)i / layer_count;

            // This is unit length
            JFloat3 tangentU(-s, 0.0f, c);

            const float dr = bottom - top;
            JFloat3 bitangent(dr * c, -height, dr * s);

            JVector4 T = DirectX::XMLoadFloat3(&tangentU);
            JVector4 B = DirectX::XMLoadFloat3(&bitangent);
            JVector4 N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(T, B));
            XMStoreFloat3(&vertex.normal, N);

            vertex.color = j % 2 == 0 ? color1 : color2;
            m_vertices.push_back(vertex);
        }
    }

    // Number of vertices in each cylinder ring
    const u32 ring_vertex_count = slice_count + 1;

    // Calculate indexes for each layer
    for (u32 i = 0; i < layer_count; ++i)
    {
        for (u32 j = 0; j < slice_count; ++j)
        {
            m_indices.push_back(i * ring_vertex_count + j);
            m_indices.push_back((i + 1) * ring_vertex_count + j);
            m_indices.push_back((i + 1) * ring_vertex_count + j + 1);
            m_indices.push_back(i * ring_vertex_count + j);
            m_indices.push_back((i + 1) * ring_vertex_count + j + 1);
            m_indices.push_back(i * ring_vertex_count + j + 1);
        }
    }

    // Constructs vertices of cylinder covers
    for (u32 k = 0; k < 2; ++k)
    {
        const u32 base_index = u32(m_vertices.size());

        const f32 y = (k - 0.5f) * height;
        const f32 theta = 2.0f * DirectX::XM_PI / slice_count;
        const f32 r = (k ? top : bottom);
        const JFloat3 normals_cover = (k ? JFloat3(0.0f, 1.0f, 0.0f) : JFloat3(0.0f, -1.0f, 0.0f));
        GeometryVertex vertex;

        for (u32 i = 0; i <= slice_count; i++)
        {
            const f32 x = r * cosf(i * theta);
            const f32 z = r * sinf(i * theta);

            // Scale down by the height to try and make top cap texture coord area proportional to base
            const f32 u = x / height + 0.5f;
            const f32 v = z / height + 0.5f;

            vertex.pos = DirectX::XMFLOAT3{ x, y, z };
            vertex.normal = normals_cover;
            vertex.tex = JFloat2(u, v);
            vertex.color = i % 2 == 0 ? color1 : color2;

            m_vertices.push_back(vertex);
        }

        // Central vertex of the lid
        vertex.pos = DirectX::XMFLOAT3{ 0.0f, y, 0.0f };

        // JFloat3 normals_center = (k ? JFloat3(0.0f, 1.0f, 0.0f) : JFloat3(0.0f, -1.0f, 0.0f));
        vertex.normal = normals_cover;

        vertex.tex = JFloat2(0.5f, 0.5f);

        vertex.color = k % 2 == 0 ? color1 : color2;
        m_vertices.push_back(vertex);

        const u32 center_index = u32(m_vertices.size() - 1);

        // Indices for the lid
        for (u32 i = 0; i < slice_count; ++i)
        {
            m_indices.push_back(center_index);
            m_indices.push_back(base_index + i + k);
            m_indices.push_back(base_index + i + 1 - k);
        }
    }
}

joj::Cylinder::Cylinder(f32 bottom, f32 top, f32 height, u32 slice_count, u32 layer_count, const JFloat4 color)
{
    m_type = GeometryType::CYLINDER;

    // Layer height
    const f32 layer_height = height / layer_count;

    // Increment in the radius of each layer
    const f32 radius_step = (top - bottom) / layer_count;

    // Number of cylinder rings
    const u32 ring_count = layer_count + 1;

    // Calculate vertices of each ring
    for (u32 i = 0; i < ring_count; ++i)
    {
        const f32 y = -0.5f * height + i * layer_height;
        const f32 r = bottom + i * radius_step;
        const f32 theta = 2.0f * DirectX::XM_PI / slice_count;

        for (u32 j = 0; j <= slice_count; ++j)
        {
            f32 c = cosf(j * theta);
            f32 s = sinf(j * theta);

            GeometryVertex vertex;
            vertex.pos = JFloat3{ r * c, y, r * s };

            vertex.tex.x = (f32)j / slice_count;
            vertex.tex.y = 1.0f - (f32)i / layer_count;

            // This is unit length
            JFloat3 tangentU(-s, 0.0f, c);

            const float dr = bottom - top;
            JFloat3 bitangent(dr * c, -height, dr * s);

            JVector4 T = DirectX::XMLoadFloat3(&tangentU);
            JVector4 B = DirectX::XMLoadFloat3(&bitangent);
            JVector4 N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(T, B));
            XMStoreFloat3(&vertex.normal, N);

            vertex.color = color;
            m_vertices.push_back(vertex);
        }
    }

    // Number of vertices in each cylinder ring
    const u32 ring_vertex_count = slice_count + 1;

    // Calculate indexes for each layer
    for (u32 i = 0; i < layer_count; ++i)
    {
        for (u32 j = 0; j < slice_count; ++j)
        {
            m_indices.push_back(i * ring_vertex_count + j);
            m_indices.push_back((i + 1) * ring_vertex_count + j);
            m_indices.push_back((i + 1) * ring_vertex_count + j + 1);
            m_indices.push_back(i * ring_vertex_count + j);
            m_indices.push_back((i + 1) * ring_vertex_count + j + 1);
            m_indices.push_back(i * ring_vertex_count + j + 1);
        }
    }

    // Constructs vertices of cylinder covers
    for (u32 k = 0; k < 2; ++k)
    {
        const u32 base_index = u32(m_vertices.size());

        const f32 y = (k - 0.5f) * height;
        const f32 theta = 2.0f * DirectX::XM_PI / slice_count;
        const f32 r = (k ? top : bottom);
        const JFloat3 normals_cover = (k ? JFloat3(0.0f, 1.0f, 0.0f) : JFloat3(0.0f, -1.0f, 0.0f));
        GeometryVertex vertex;

        for (u32 i = 0; i <= slice_count; i++)
        {
            const f32 x = r * cosf(i * theta);
            const f32 z = r * sinf(i * theta);

            // Scale down by the height to try and make top cap texture coord area proportional to base
            const f32 u = x / height + 0.5f;
            const f32 v = z / height + 0.5f;

            vertex.pos = DirectX::XMFLOAT3{ x, y, z };
            vertex.normal = normals_cover;
            vertex.tex = JFloat2(u, v);
            vertex.color = color;
            m_vertices.push_back(vertex);
        }

        // Central vertex of the lid
        vertex.pos = DirectX::XMFLOAT3{ 0.0f, y, 0.0f };

        // JFloat3 normals_center = (k ? JFloat3(0.0f, 1.0f, 0.0f) : JFloat3(0.0f, -1.0f, 0.0f));
        vertex.normal = normals_cover;

        vertex.tex = JFloat2(0.5f, 0.5f);

        vertex.color = color;
        m_vertices.push_back(vertex);

        const u32 center_index = u32(m_vertices.size() - 1);

        // Indices for the lid
        for (u32 i = 0; i < slice_count; ++i)
        {
            m_indices.push_back(center_index);
            m_indices.push_back(base_index + i + k);
            m_indices.push_back(base_index + i + 1 - k);
        }
    }
}

void joj::Cylinder::build_top_cap(f32 bottom_radius, f32 top_radius, f32 height, u32 slice_count, u32 stack_count, const JFloat4 color)
{
    const JFloat4 color1(1.0f, 0.0f, 0.0f, 1.0f);     // Red
    const JFloat4 color2(1.0f, 1.0f, 0.0f, 1.0f);     // Yellow
    JFloat4 base_color(0.0f, 0.0f, 0.0f, 1.0f); // Black

    const u32 base_index = u32(m_vertices.size());

    const f32 y = 0.5f * height;
    const f32 theta = 2.0f * DirectX::XM_PI / slice_count;

    // Duplicate cap ring vertices
    for (u32 i = 0; i <= slice_count; ++i)
    {
        const f32 x = top_radius * cosf(i * theta);
        const f32 z = top_radius * sinf(i * theta);

        // Scale down by the height to try and make top cap texture coord area proportional to base
        const f32 u = x / height + 0.5f;
        const f32 v = z / height + 0.5f;

        base_color = i % 2 == 0 ? color1 : color2;

        m_vertices.push_back(GeometryVertex(x, y, z, 0.0f, 1.0f, 0.0f, base_color.x, base_color.y, base_color.z, base_color.w, u, v));
    }

    base_color = base_index % 2 == 0 ? color1 : color2;

    // Cap center vertex
    m_vertices.push_back(GeometryVertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, base_color.x, base_color.y, base_color.z, base_color.w, 0.5f, 0.5f));

    // Index of center vertex.
    const u32 center_index = (u32)m_vertices.size() - 1;

    for (u32 i = 0; i < slice_count; ++i)
    {
        m_indices.push_back(center_index);
        m_indices.push_back(base_index + i + 1);
        m_indices.push_back(base_index + i);
    }
}

void joj::Cylinder::build_bottom_cap(f32 bottom_radius, f32 top_radius, f32 height, u32 slice_count, u32 stack_count, const JFloat4 color)
{
    const JFloat4 color1(1.0f, 0.0f, 0.0f, 1.0f);     // Red
    const JFloat4 color2(1.0f, 1.0f, 0.0f, 1.0f);     // Yellow
    JFloat4 base_color(0.0f, 0.0f, 0.0f, 1.0f); // Black

    const u32 base_index = (u32)m_vertices.size();
    const f32 y = -0.5f * height;

    // vertices of ring
    const f32 theta = 2.0f * J_PI / slice_count;

    for (u32 i = 0; i <= slice_count; ++i)
    {
        const f32 x = bottom_radius * cosf(i * theta);
        const f32 z = bottom_radius * sinf(i * theta);

        // Scale down by the height to try and make top cap texture coord area proportional to base
        const f32 u = x / height + 0.5f;
        const f32 v = z / height + 0.5f;

        base_color = i % 2 == 0 ? color1 : color2;

        m_vertices.push_back(GeometryVertex(x, y, z, 0.0f, -1.0f, 0.0f, base_color.x, base_color.y, base_color.z, base_color.w, u, v));
    }

    base_color = base_index % 2 == 0 ? color1 : color2;

    // Cap center vertex.
    m_vertices.push_back(GeometryVertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, base_color.x, base_color.y, base_color.z, base_color.w, 0.5f, 0.5f));

    // Cache the index of center vertex.
    const u32 centerIndex = (u32)m_vertices.size() - 1;

    for (u32 i = 0; i < slice_count; ++i)
    {
        m_indices.push_back(centerIndex);
        m_indices.push_back(base_index + i);
        m_indices.push_back(base_index + i + 1);
    }
}