#include "systems/objects2d/object2d.h"

joj::Object2D::Object2D()
    : m_pos(0.0f, 0.0f), m_size(0.0f, 0.0f)
{
}

joj::Object2D::Object2D(f32 xpos, f32 ypos, f32 xsize, f32 ysize)
    : m_pos(xpos, ypos), m_size(xsize, ysize)
{
}

joj::Object2D::~Object2D()
{
}