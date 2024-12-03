#include "renderer/d3d11/sprite_animation_d3d11.h"

#if JPLATFORM_WINDOWS

joj::D3D11SpriteAnimation::D3D11SpriteAnimation()
    : m_delay(0.0f), m_loop(false), m_time_elapsed(0.0f),
    m_current_frame(0), m_total_frames(0)
{
}

joj::D3D11SpriteAnimation::D3D11SpriteAnimation(u32 frames_per_second, b8 loop)
    : m_delay{1.0f / static_cast<f32>(frames_per_second)}, m_loop(loop), m_time_elapsed(0.0f),
    m_current_frame(0), m_total_frames(0)
{
}

joj::D3D11SpriteAnimation::~D3D11SpriteAnimation()
{
}

void joj::D3D11SpriteAnimation::add_frame(u32 id, D3D11AnimationFrame* frames, u32 size)
{
    AnimationSequence new_seq(new D3D11AnimationFrame[size], size);

    memcpy(new_seq.first, frames, sizeof(D3D11AnimationFrame) * size);

    m_frames[id] = new_seq;

    m_total_frames = size;
}

void joj::D3D11SpriteAnimation::update(const f32 dt)
{
    if (!m_loop && m_current_frame >= m_total_frames)
        return;

    m_time_elapsed += dt;

    if (m_time_elapsed >= m_delay)
    {
        m_current_frame = (m_current_frame + 1) % m_total_frames;
        m_time_elapsed = 0.0f;
    }
}

#endif // JPLATFORM_WINDOWS