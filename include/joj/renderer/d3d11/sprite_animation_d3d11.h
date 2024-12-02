#ifndef JOJ_D3D11_SPRITE_ANIMATION_H
#define JOJ_D3D11_SPRITE_ANIMATION_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "math/jmath.h"
#include <unordered_map> 
#include "tileset_d3d11.h"

namespace joj
{
    using AnimationSequence = std::pair<u32*, u32>;

    class JAPI D3D11SpriteAnimation
    {
    public:
        D3D11SpriteAnimation();
        D3D11SpriteAnimation(D3D11Tileset* tileset, f32 delay, b8 loop);
        ~D3D11SpriteAnimation();

        void add(u32 id, u32* sequence, u32 sequence_size);

        void select(u32 id);
        void update(const f32 dt);
        void next_frame();
        void calculate_tex_coord();

        const JFloat2& get_tex_coord() const;
        const JFloat2& get_tex_size() const;

    private:
        i32 m_start_frame;
        i32 m_end_frame;
        i32 m_current_frame;
        b8 m_loop;
        f32 m_delay;
        f32 m_time_elapsed;
        D3D11Tileset* m_tileset;
        u32* m_sequence;

        std::unordered_map<u32, AnimationSequence> m_table;

        // Sprite Data
        u32 m_sprite_width;
        u32 m_sprite_height;
        JFloat2 m_tex_coord;
        JFloat2 m_tex_size;
    };

    inline const JFloat2& D3D11SpriteAnimation::get_tex_coord() const
    { return m_tex_coord; }

    inline const JFloat2& D3D11SpriteAnimation::get_tex_size() const
    { return m_tex_size; }
}

#endif // JOJ_D3D11_SPRITE_ANIMATION_H