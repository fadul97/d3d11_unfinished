#include "renderer/d3d11/sprite_animation_d3d11.h"

joj::D3D11SpriteAnimation::D3D11SpriteAnimation()
{
    m_start_frame = 0;
    m_current_frame = 0;
    m_end_frame = 0;
    m_time_elapsed = 0.0f;
    m_sequence = nullptr;
    m_tileset = nullptr;

    m_delay = 0.0f;
    m_loop = false;

    // Sprite Data
    m_sprite_width = 0;
    m_sprite_height = 0;
    m_tex_coord = { 0.0f, 0.0f };
    m_tex_size = { 1.0f, 1.0f };
}

joj::D3D11SpriteAnimation::D3D11SpriteAnimation(D3D11Tileset* tileset, f32 delay, b8 loop)
    : m_tileset(tileset), m_delay(delay), m_loop(loop)
{
    m_start_frame = 0;
    m_current_frame = 0;
    m_end_frame = tileset->get_size() - 1;
    m_time_elapsed = 0.0f;
    m_sequence = nullptr;

    // Sprite Data
    m_sprite_width = tileset->get_tile_width();
    m_sprite_height = tileset->get_tile_height();
    m_tex_coord = { 0.0f, 0.0f };
    m_tex_size.x = static_cast<f32>(tileset->get_tile_width()) / static_cast<f32>(tileset->get_width());
    m_tex_size.y = static_cast<f32>(tileset->get_tile_height()) / static_cast<f32>(tileset->get_height());
}

joj::D3D11SpriteAnimation::~D3D11SpriteAnimation()
{
    if (!m_table.empty())
    {
        for (const auto& anim_seq : m_table)
            delete anim_seq.second.first;
    }
}

void joj::D3D11SpriteAnimation::add(u32 id, u32* sequence, u32 sequence_size)
{
    // Create new animation sequence
    AnimationSequence new_anim_seq(new u32[sequence_size], sequence_size);

    // Copy vector with frame sequence
    memcpy(new_anim_seq.first, sequence, sizeof(u32) * sequence_size);

    // Insert new sequence
    m_table[id] = new_anim_seq;

    // Select new inserted sequence
    m_sequence = new_anim_seq.first;

    // Initialize animation with first sequence frame
    m_start_frame = m_current_frame = 0;

    // Last frame is always 1 - number of frames
    m_end_frame = sequence_size - 1;
}

void joj::D3D11SpriteAnimation::select(u32 id)
{
    const auto& e = m_table[id];

    if (m_sequence != e.first)
    {
        m_sequence = e.first;

        m_start_frame = 0;
        m_end_frame = e.second - 1;

        if (m_current_frame > m_end_frame)
            m_current_frame = 0;
    }
}

void joj::D3D11SpriteAnimation::update(const f32 dt)
{
    m_time_elapsed += dt;

    if (m_time_elapsed >= m_delay)
    {
        next_frame();
    }
}

void joj::D3D11SpriteAnimation::next_frame()
{
    ++m_current_frame;

    if (m_current_frame > m_end_frame)
    {
        if (m_loop)
        {
            m_current_frame = 0;
            m_time_elapsed = 0.0f;
        }
        else
        {
            m_current_frame = m_end_frame;
        }
    }

    calculate_tex_coord();
}

void joj::D3D11SpriteAnimation::calculate_tex_coord()
{
    m_tex_coord.x = (m_current_frame % m_tileset->get_columns()) * m_tex_size.x;
    m_tex_coord.y = (m_current_frame / m_tileset->get_columns()) * m_tex_size.y;
}