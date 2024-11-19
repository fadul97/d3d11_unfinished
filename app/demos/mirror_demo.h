#ifndef MIRROR_DEMO_H
#define MIRROR_DEMO_H

// ---------------------------------------------------------------------------------

#include "joj/application/app.h"
#include "joj/renderer/d3d11/shader_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include <renderer/d3d11/index_buffer_d3d11.h>
#include <renderer/d3d11/constant_buffer_d3d11.h>
#include "joj/math/jmath.h"
#include "joj/systems/camera/free_camera.h"
#include "joj/systems/light/light.h"
#include "joj/systems/material/material.h"
#include <vector>

// ---------------------------------------------------------------------------------

enum class RenderOptions
{
	Lighting,
	Textures,
	TexturesAndFog
};

// ---------------------------------------------------------------------------------

class MirrorDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

public:
	joj::JFloat4x4 m_room_world = joj::float4x4_identity();
	joj::JFloat4x4 m_skull_world = joj::float4x4_identity();
	joj::JFloat3 m_skull_translation{ 0.0f, 1.0f, -5.0f };

	joj::DirectionalLight m_dir_lights[3];
	joj::SimpleMaterial m_room_mat;
	joj::SimpleMaterial m_skull_mat;
	joj::SimpleMaterial m_mirror_mat;
	joj::SimpleMaterial m_shadow_mat;

	ID3D11RasterizerState* m_wireframe_RS = nullptr;
	ID3D11RasterizerState* m_no_cull_RS = nullptr;
	ID3D11RasterizerState* m_cull_clock_wise_RS = nullptr;
	ID3D11BlendState* m_alpha_to_coverage_BS = nullptr;
	ID3D11BlendState* m_transparent_BS = nullptr;
	ID3D11BlendState* m_no_render_target_write_BS = nullptr;
	ID3D11DepthStencilState* m_mark_mirror_DSS = nullptr;
	ID3D11DepthStencilState* m_draw_reflection_DSS = nullptr;
	ID3D11DepthStencilState* m_no_double_blend_DSS = nullptr;
	void build_render_states();

	ID3D11ShaderResourceView* m_floor_diffuse_map_SRV = nullptr;
	ID3D11ShaderResourceView* m_wall_diffuse_map_SRV = nullptr;
	ID3D11ShaderResourceView* m_mirror_diffuse_map_SRV = nullptr;
	ID3D11SamplerState* m_sampler_state = nullptr;
	void build_textures();

	joj::D3D11VertexBuffer m_room_vb;
	void build_room_geometry_buffers();

	joj::D3D11VertexBuffer m_skull_vb;
	joj::D3D11IndexBuffer m_skull_ib;
	i32 m_skull_index_count = 0;
	void build_skull_geometry_buffers();

	joj::D3D11Shader m_shader;
	void build_shaders();

	std::vector<D3D11_INPUT_ELEMENT_DESC> m_input_desc;
	ID3D11InputLayout* m_input_layout = nullptr;
	void build_vertex_layout();

	joj::D3D11ConstantBuffer m_object_cb;
	joj::D3D11ConstantBuffer m_frame_cb;
	void build_constant_buffers();

	joj::FreeCamera camera;
	b8 firstPerson = true;
	b8 hideCursor = false;
	i32 centerX = 400;
	i32 centerY = 300;
	b8 fast = false;
	f32 m_camera_speed = 10.0f;
	b8 is_wireframe = false;

	joj::JFloat4x4 mView = joj::float4x4_identity();
	joj::JFloat4x4 mProj = joj::float4x4_identity();

	f64 prev = 0.0f;
	f32 t_base = 0.0f;
	RenderOptions m_render_options = RenderOptions::Lighting;
	i32 m_light_count = 2;
};

// ---------------------------------------------------------------------------------

#endif // MIRROR_DEMO_H