#include "renderer/d3d11/sprite_animation_d3d11.h"

#if JPLATFORM_WINDOWS

joj::D3D11SpriteAnimation::D3D11SpriteAnimation()
    : m_total_frames(0), m_columns(0), m_rows(0), m_frame_duration(0.0f),
    m_current_frame(0), m_time_elapsed(0.0f), m_uv_offset(0.0f, 0.0f),
    m_cell_size(1.0f, 1.0f)
{
}

joj::D3D11SpriteAnimation::D3D11SpriteAnimation(i32 total_frames, i32 columns, i32 rows, f32 frame_duration)
    : m_total_frames(total_frames), m_columns(columns), m_rows(rows), m_frame_duration(frame_duration)
{
    m_current_frame = 0;
    m_time_elapsed = 0.0f;
    m_uv_offset = { 0.0f, 0.0f };
    m_cell_size = { 1.0f / static_cast<f32>(columns), 1.0f / static_cast<f32>(rows) };
}

joj::D3D11SpriteAnimation::~D3D11SpriteAnimation()
{
}

void joj::D3D11SpriteAnimation::update(const f32 dt)
{
    m_time_elapsed += dt;

    if (m_time_elapsed >= m_frame_duration)
    {
        m_current_frame = (m_current_frame + 1) % m_total_frames;

        i32 column = m_current_frame % m_columns;
        i32 row = m_current_frame / m_columns;
        m_uv_offset = { column * m_cell_size.x, row * m_cell_size.y };

        m_time_elapsed = 0.0f;
    }
}

#endif // JPLATFORM_WINDOWS