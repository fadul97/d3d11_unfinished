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
	u32 m_current_frame = 0;
	u32 m_total_frames = 120;
	f32 time_elapsed = 0.0f;
	f32 m_cell_width = 1.0f / 10.0f;
	f32 m_cell_height = 1.0f / 12.0f;
};

#endif // ENDLESS_RUNNER_H