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
#include "joj/systems/objects2d/sprite_animation.h"

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
	joj::D3D11Texture m_player_tex;
	joj::SpriteAnimation m_running;
	joj::SpriteAnimation m_inverted;
	b8 inverted = false;
};

#endif // ENDLESS_RUNNER_H