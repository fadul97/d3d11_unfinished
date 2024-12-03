#ifndef ENDLESS_RUNNER_H
#define ENDLESS_RUNNER_H

#include "joj/application/app.h"
#include "joj/renderer/d3d11/shader_d3d11.h"
#include <renderer/d3d11/input_layout_d3d11.h>
#include "joj/renderer/d3d11/sampler_state_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include "joj/renderer/d3d11/index_buffer_d3d11.h"
#include "joj/renderer/d3d11/constant_buffer_d3d11.h"
#include <renderer/d3d11/texture_d3d11.h>
#include "joj/math/jmath.h"
#include <renderer/d3d11/tileset_d3d11.h>
#include <renderer/d3d11/sprite_animation_d3d11.h>
#include <renderer/d3d11/sprite_sheet_d3d11.h>
#include <renderer/d3d11/animation_frame_d3d11.h>

class EndlessRunner : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

private:
	joj::D3D11Shader m_shader;
	joj::D3D11InputLayout m_input_layout;
	joj::D3D11SamplerState m_sampler_state;
	joj::D3D11VertexBuffer vb;
	joj::D3D11IndexBuffer ib;
	joj::D3D11ConstantBuffer cb;
	joj::D3D11SpriteSheet m_fire_ss;
	joj::D3D11SpriteAnimation m_fire_animation;
	joj::D3D11SpriteSheet m_ship_ss;
	joj::D3D11SpriteAnimation m_ship_animation;

	joj::D3D11SpriteSheet m_guy_ss;
	joj::D3D11SpriteAnimation m_guy;
};

#endif // ENDLESS_RUNNER_H