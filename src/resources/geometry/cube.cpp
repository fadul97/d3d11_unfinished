#include "resources/geometry/cube.h"

joj::Cube::Cube()
{
    m_type = GeometryType::CUBE;
}

joj::Cube::Cube(f32 width, f32 height, f32 depth)
{
    m_type = GeometryType::CUBE;

    const f32 w = 0.5f * width;
    const f32 h = 0.5f * height;
    const f32 d = 0.5f * depth;

    const JFloat4 color1(1.0f, 0.0f, 0.0f, 1.0f); // Red
    const JFloat4 color2(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

    // Create geometry vertices
    GeometryVertex cube_vertices[24] =
    {
        // Fill in the front face vertex data.
        GeometryVertex(-w, -h, -d, 0.0f, 0.0f, -1.0f, color1.x, color1.y, color1.z, color1.w, 0.0f, 1.0f),
        GeometryVertex(-w, +h, -d, 0.0f, 0.0f, -1.0f, color2.x, color2.y, color2.z, color2.w, 0.0f, 0.0f),
        GeometryVertex(+w, +h, -d, 0.0f, 0.0f, -1.0f, color1.x, color1.y, color1.z, color1.w, 1.0f, 0.0f),
        GeometryVertex(+w, -h, -d, 0.0f, 0.0f, -1.0f, color2.x, color2.y, color2.z, color2.w, 1.0f, 1.0f),

        // Fill in the back face vertex data.
        GeometryVertex(-w, -h, +d, 0.0f, 0.0f, 1.0f, color2.x, color2.y, color2.z, color2.w, 1.0f, 1.0f),
        GeometryVertex(+w, -h, +d, 0.0f, 0.0f, 1.0f, color1.x, color1.y, color1.z, color1.w, 0.0f, 1.0f),
        GeometryVertex(+w, +h, +d, 0.0f, 0.0f, 1.0f, color2.x, color2.y, color2.z, color2.w, 0.0f, 0.0f),
        GeometryVertex(-w, +h, +d, 0.0f, 0.0f, 1.0f, color1.x, color1.y, color1.z, color1.w, 1.0f, 0.0f),

        // Fill in the top face vertex data.
        GeometryVertex(-w, +h, -d, 0.0f, 1.0f, 0.0f, color1.x, color1.y, color1.z, color1.w, 0.0f, 1.0f),
        GeometryVertex(-w, +h, +d, 0.0f, 1.0f, 0.0f, color2.x, color2.y, color2.z, color2.w, 0.0f, 0.0f),
        GeometryVertex(+w, +h, +d, 0.0f, 1.0f, 0.0f, color1.x, color1.y, color1.z, color1.w, 1.0f, 0.0f),
        GeometryVertex(+w, +h, -d, 0.0f, 1.0f, 0.0f, color2.x, color2.y, color2.z, color2.w, 1.0f, 1.0f),

        // Fill in the bottom face vertex data.
        GeometryVertex(-w, -h, -d, 0.0f, -1.0f, 0.0f, color2.x, color2.y, color2.z, color2.w, 1.0f, 1.0f),
        GeometryVertex(+w, -h, -d, 0.0f, -1.0f, 0.0f, color1.x, color1.y, color1.z, color1.w, 0.0f, 1.0f),
        GeometryVertex(+w, -h, +d, 0.0f, -1.0f, 0.0f, color2.x, color2.y, color2.z, color2.w, 0.0f, 0.0f),
        GeometryVertex(-w, -h, +d, 0.0f, -1.0f, 0.0f, color1.x, color1.y, color1.z, color1.w, 1.0f, 0.0f),

        // Fill in the left face vertex data.
        GeometryVertex(-w, -h, +d, -1.0f, 0.0f, 0.0f, color1.x, color1.y, color1.z, color1.w, 0.0f, 1.0f),
        GeometryVertex(-w, +h, +d, -1.0f, 0.0f, 0.0f, color2.x, color2.y, color2.z, color2.w, 0.0f, 0.0f),
        GeometryVertex(-w, +h, -d, -1.0f, 0.0f, 0.0f, color1.x, color1.y, color1.z, color1.w, 1.0f, 0.0f),
        GeometryVertex(-w, -h, -d, -1.0f, 0.0f, 0.0f, color2.x, color2.y, color2.z, color2.w, 1.0f, 1.0f),

        // Fill in the right face vertex data.
        GeometryVertex(+w, -h, -d, 1.0f, 0.0f, 0.0f, color2.x, color2.y, color2.z, color2.w, 0.0f, 1.0f),
        GeometryVertex(+w, +h, -d, 1.0f, 0.0f, 0.0f, color1.x, color1.y, color1.z, color1.w, 0.0f, 0.0f),
        GeometryVertex(+w, +h, +d, 1.0f, 0.0f, 0.0f, color2.x, color2.y, color2.z, color2.w, 1.0f, 0.0f),
        GeometryVertex(+w, -h, +d, 1.0f, 0.0f, 0.0f, color1.x, color1.y, color1.z, color1.w, 1.0f, 1.0f)
    };

    // Add vertices to mesh
    for (const GeometryVertex& v : cube_vertices)
        m_vertices.push_back(v);

    // Indicate how vertices are interconnected
    u32 cube_indices[36] =
    {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 5, 6,
        4, 6, 7,

        // top face
        8, 9, 10,
        8, 10, 11,

        // bottom face
        12, 13, 14,
        12, 14, 15,

        // left face
        16, 17, 18,
        16, 18, 19,

        // right face
        20, 21, 22,
        20, 22, 23
    };

    // Add indices to mesh
    m_indices.insert(m_indices.end(), std::begin(cube_indices), std::end(cube_indices));

    /*
    // Add indices to mesh
    for (u32 i : cube_indices)
        m_indices.push_back(i);
    */
}

joj::Cube::Cube(f32 width, f32 height, f32 depth, const JFloat4 color)
{
    m_type = GeometryType::CUBE;

    const f32 w = 0.5f * width;
    const f32 h = 0.5f * height;
    const f32 d = 0.5f * depth;

    // Create geometry vertices
    GeometryVertex cube_vertices[24] =
    {
        // Fill in the front face vertex data.
        GeometryVertex(-w, -h, -d, 0.0f, 0.0f, -1.0f, color.x, color.y, color.z, color.w, 0.0f, 1.0f),
        GeometryVertex(-w, +h, -d, 0.0f, 0.0f, -1.0f, color.x, color.y, color.z, color.w, 0.0f, 0.0f),
        GeometryVertex(+w, +h, -d, 0.0f, 0.0f, -1.0f, color.x, color.y, color.z, color.w, 1.0f, 0.0f),
        GeometryVertex(+w, -h, -d, 0.0f, 0.0f, -1.0f, color.x, color.y, color.z, color.w, 1.0f, 1.0f),

        // Fill in the back face vertex data.
        GeometryVertex(-w, -h, +d, 0.0f, 0.0f, 1.0f, color.x, color.y, color.z, color.w, 1.0f, 1.0f),
        GeometryVertex(+w, -h, +d, 0.0f, 0.0f, 1.0f, color.x, color.y, color.z, color.w, 0.0f, 1.0f),
        GeometryVertex(+w, +h, +d, 0.0f, 0.0f, 1.0f, color.x, color.y, color.z, color.w, 0.0f, 0.0f),
        GeometryVertex(-w, +h, +d, 0.0f, 0.0f, 1.0f, color.x, color.y, color.z, color.w, 1.0f, 0.0f),

        // Fill in the top face vertex data.
        GeometryVertex(-w, +h, -d, 0.0f, 1.0f, 0.0f, color.x, color.y, color.z, color.w, 0.0f, 1.0f),
        GeometryVertex(-w, +h, +d, 0.0f, 1.0f, 0.0f, color.x, color.y, color.z, color.w, 0.0f, 0.0f),
        GeometryVertex(+w, +h, +d, 0.0f, 1.0f, 0.0f, color.x, color.y, color.z, color.w, 1.0f, 0.0f),
        GeometryVertex(+w, +h, -d, 0.0f, 1.0f, 0.0f, color.x, color.y, color.z, color.w, 1.0f, 1.0f),

        // Fill in the bottom face vertex data.
        GeometryVertex(-w, -h, -d, 0.0f, -1.0f, 0.0f, color.x, color.y, color.z, color.w, 1.0f, 1.0f),
        GeometryVertex(+w, -h, -d, 0.0f, -1.0f, 0.0f, color.x, color.y, color.z, color.w, 0.0f, 1.0f),
        GeometryVertex(+w, -h, +d, 0.0f, -1.0f, 0.0f, color.x, color.y, color.z, color.w, 0.0f, 0.0f),
        GeometryVertex(-w, -h, +d, 0.0f, -1.0f, 0.0f, color.x, color.y, color.z, color.w, 1.0f, 0.0f),

        // Fill in the left face vertex data.
        GeometryVertex(-w, -h, +d, -1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w, 0.0f, 1.0f),
        GeometryVertex(-w, +h, +d, -1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w, 0.0f, 0.0f),
        GeometryVertex(-w, +h, -d, -1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w, 1.0f, 0.0f),
        GeometryVertex(-w, -h, -d, -1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w, 1.0f, 1.0f),

        // Fill in the right face vertex data.
        GeometryVertex(+w, -h, -d, 1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w, 0.0f, 1.0f),
        GeometryVertex(+w, +h, -d, 1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w, 0.0f, 0.0f),
        GeometryVertex(+w, +h, +d, 1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w, 1.0f, 0.0f),
        GeometryVertex(+w, -h, +d, 1.0f, 0.0f, 0.0f, color.x, color.y, color.z, color.w, 1.0f, 1.0f)
    };

    // Add vertices to mesh
    for (const GeometryVertex& v : cube_vertices)
        m_vertices.push_back(v);

    // Indicate how vertices are interconnected
    u32 cube_indices[36] =
    {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 5, 6,
        4, 6, 7,

        // top face
        8, 9, 10,
        8, 10, 11,

        // bottom face
        12, 13, 14,
        12, 14, 15,

        // left face
        16, 17, 18,
        16, 18, 19,

        // right face
        20, 21, 22,
        20, 22, 23
    };

    // Add indices to mesh
    for (u16 i : cube_indices)
        m_indices.push_back(i);
}