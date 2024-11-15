#include "resources/geometry/quad.h"

joj::Quad::Quad()
{
    m_type = GeometryType::QUAD;
}

joj::Quad::Quad(f32 width, f32 height)
{
    m_type = GeometryType::QUAD;

    const JFloat4 color1(1.0f, 0.0f, 0.0f, 1.0f);  // Red
    const JFloat4 color2(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow

    const f32 w = 0.5f * width;
    const f32 h = 0.5f * height;

    // Create vertex buffer
    const GeometryVertex quad_vertices[4] =
    {
        GeometryVertex(-w, -h, 0.0f, 0.0f, 0.0f, -1.0f, color1.x, color1.y, color1.z, color1.w, 0.0f, 1.0f),
        GeometryVertex(-w, +h, 0.0f, 0.0f, 0.0f, -1.0f, color2.x, color2.y, color2.z, color2.w, 0.0f, 0.0f),
        GeometryVertex(+w, +h, 0.0f, 0.0f, 0.0f, -1.0f, color2.x, color2.y, color2.z, color2.w, 1.0f, 0.0f),
        GeometryVertex(+w, -h, 0.0f, 0.0f, 0.0f, -1.0f, color1.x, color1.y, color1.z, color1.w, 1.0f, 1.0f),
    };

    // Add vertices to mesh
    for (const GeometryVertex& v : quad_vertices)
        m_vertices.push_back(v);

    // Create index buffer
    const u32 quad_indices[6] =
    {
        0, 1, 2,
        0, 2, 3
    };

    // Add indices to mesh
    for (u32 i : quad_indices)
        m_indices.push_back(i);

}

joj::Quad::Quad(f32 width, f32 height, const JFloat4 color)
{
    m_type = GeometryType::QUAD;

    const f32 w = 0.5f * width;
    const f32 h = 0.5f * height;

    // Create vertex buffer
    const GeometryVertex quad_vertices[4] =
    {
        GeometryVertex(-w, -h, 0.0f, 0.0f, 0.0f, -1.0f, color.x, color.y, color.z, color.w, 0.0f, 1.0f),
        GeometryVertex(-w, +h, 0.0f, 0.0f, 0.0f, -1.0f, color.x, color.y, color.z, color.w, 0.0f, 0.0f),
        GeometryVertex(+w, +h, 0.0f, 0.0f, 0.0f, -1.0f, color.x, color.y, color.z, color.w, 1.0f, 0.0f),
        GeometryVertex(+w, -h, 0.0f, 0.0f, 0.0f, -1.0f, color.x, color.y, color.z, color.w, 1.0f, 1.0f),
    };

    // Add vertices to mesh
    for (const GeometryVertex& v : quad_vertices)
        m_vertices.push_back(v);

    // Create index buffer
    const u32 quad_indices[6] =
    {
        0, 1, 2,
        0, 2, 3
    };

    // Add indices to mesh
    for (u32 i : quad_indices)
        m_indices.push_back(i);
}