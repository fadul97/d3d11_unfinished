#include "systems/objects2d/sprite_animation.h"

joj::SpriteAnimation::SpriteAnimation()
    : m_columns(0), m_rows(0), m_delay(0.0f), m_loop(false),
    m_current_frame(0), m_time_elapsed(0.0f), m_end_frame(0),
    m_start_frame(0), m_tex_coord(0.0f, 0.0f), m_tex_size(1.0f, 1.0f)
{
}

joj::SpriteAnimation::SpriteAnimation(i32 rows, i32 columns, i32 start_frame, i32 end_frame, f32 delay, b8 loop)
    : m_columns(columns), m_rows(rows), m_start_frame(start_frame), m_end_frame(end_frame), m_delay(delay),
    m_loop(loop), m_current_frame(start_frame), m_time_elapsed(0.0f)
{
    m_tex_coord = { 0.0f, 0.0f };
    m_tex_size = { 1.0f / static_cast<float>(columns), 1.0f / static_cast<float>(rows) };
}

joj::SpriteAnimation::~SpriteAnimation()
{
}

void joj::SpriteAnimation::update(const f32 dt)
{
    m_time_elapsed += dt;

    if (m_time_elapsed >= m_delay)
    {
        next_frame();

        m_time_elapsed = 0.0f;
    }
}

void joj::SpriteAnimation::next_frame()
{
    ++m_current_frame;

    if (m_current_frame > m_end_frame)
    {
        if (m_loop)
            m_current_frame = m_start_frame;
        else
            m_current_frame = m_end_frame;
    }

    calculate_tex_coord();
}

void joj::SpriteAnimation::calculate_tex_coord()
{
    // Calcula a coluna e linha no atlas com base no número do quadro
    i32 column = m_current_frame % m_columns;
    i32 row = m_current_frame / m_columns;

    // Atualiza as coordenadas de textura
    m_tex_coord.x = column * m_tex_size.x;  // Coordenada X da célula no atlas
    m_tex_coord.y = row * m_tex_size.y;     // Coordenada Y da célula no atlas
}