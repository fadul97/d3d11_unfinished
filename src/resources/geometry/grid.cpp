#include "resources/geometry/grid.h"

joj::Grid::Grid()
{
    m_type = GeometryType::GRID;
}

joj::Grid::Grid(f32 width, f32 depth, u32 m, u32 n)
{
    m_type = GeometryType::GRID;

    const JFloat4 color1(1.0f, 0.0f, 0.0f, 1.0f);  // Red
    const JFloat4 color2(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow

    const u32 vertex_count = m * n;
    const u32 triangle_count = 2 * (m - 1) * (n - 1);

    // Create vertices
    const f32 half_width = 0.5f * width;
    const f32 half_depth = 0.5f * depth;

    const f32 dx = width / (n - 1);
    const f32 dz = depth / (m - 1);

    const f32 du = 1.0f / (n - 1);
    const f32 dv = 1.0f / (m - 1);

    // Adjust vertex vector size
    m_vertices.resize(vertex_count);

    for (u32 i = 0; i < m; ++i)
    {
        const f32 z = half_depth - i * dz;

        for (u32 j = 0; j < n; ++j)
        {
            const f32 x = -half_width + j * dx;

            // Define grid vertices
            m_vertices[size_t(i) * n + j].pos = DirectX::XMFLOAT3{ x, 0.0f, z };
            m_vertices[size_t(i) * n + j].normal = DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f };
            m_vertices[size_t(i) * n + j].color = i % 2 == 0 ? color1 : color2;

            // Stretch texture over grid
            m_vertices[i * n + j].tex.x = j * du;
            m_vertices[i * n + j].tex.y = i * dv;
        }
    }

    // Adjust vector size of indexes
    m_indices.resize(size_t(triangle_count) * 3);

    size_t k = 0;

    for (u32 i = 0; i < m - 1; ++i)
    {
        for (u32 j = 0; j < n - 1; ++j)
        {
            m_indices[k] = i * n + j;
            m_indices[k + 1] = i * n + j + 1;
            m_indices[k + 2] = (i + 1) * n + j;
            m_indices[k + 3] = (i + 1) * n + j;
            m_indices[k + 4] = i * n + j + 1;
            m_indices[k + 5] = (i + 1) * n + j + 1;

            k += 6; // Next quad
        }
    }
}

joj::Grid::Grid(f32 width, f32 depth, u32 m, u32 n, const JFloat4 color)
{
    m_type = GeometryType::GRID;

    const u32 vertex_count = m * n;
    const u32 triangle_count = 2 * (m - 1) * (n - 1);

    // Create vertices
    const f32 half_width = 0.5f * width;
    const f32 half_depth = 0.5f * depth;

    const f32 dx = width / (n - 1);
    const f32 dz = depth / (m - 1);

    const f32 du = 1.0f / (n - 1);
    const f32 dv = 1.0f / (m - 1);

    // Adjust vertex vector size
    m_vertices.resize(vertex_count);

    for (u32 i = 0; i < m; ++i)
    {
        const f32 z = half_depth - i * dz;

        for (u32 j = 0; j < n; ++j)
        {
            const f32 x = -half_width + j * dx;

            // Define grid vertices
            m_vertices[size_t(i) * n + j].pos = DirectX::XMFLOAT3{ x, 0.0f, z };
            m_vertices[size_t(i) * n + j].normal = DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f };
            m_vertices[size_t(i) * n + j].color = color;

            // Stretch texture over grid
            m_vertices[i * n + j].tex.x = j * du;
            m_vertices[i * n + j].tex.y = i * dv;
        }
    }

    // Adjust vector size of indexes
    m_indices.resize(size_t(triangle_count) * 3);

    size_t k = 0;

    for (u32 i = 0; i < m - 1; ++i)
    {
        for (u32 j = 0; j < n - 1; ++j)
        {
            m_indices[k] = i * n + j;
            m_indices[k + 1] = i * n + j + 1;
            m_indices[k + 2] = (i + 1) * n + j;
            m_indices[k + 3] = (i + 1) * n + j;
            m_indices[k + 4] = i * n + j + 1;
            m_indices[k + 5] = (i + 1) * n + j + 1;

            k += 6; // Next quad
        }
    }
}