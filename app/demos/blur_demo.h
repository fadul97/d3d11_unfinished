#ifndef BLUR_DEMO_H
#define BLUR_DEMO_H

// ---------------------------------------------------------------------------------

#include "joj/application/app.h"
#include "joj/renderer/d3d11/shader_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include <renderer/d3d11/index_buffer_d3d11.h>
#include <renderer/d3d11/constant_buffer_d3d11.h>
#include "joj/math/jmath.h"
#include "joj/systems/camera/free_camera.h"
#include "waves.h"
#include "joj/systems/light/light.h"
#include "joj/systems/material/material.h"
#include "joj/renderer/d3d11/render_state_d3d11.h"
#include "joj/renderer/d3d11/blur_filter_d3d11.h"

// ---------------------------------------------------------------------------------

class BlurDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

public:
	joj::D3D11VertexBuffer m_land_vb;
	joj::D3D11IndexBuffer m_land_ib;

	joj::D3D11VertexBuffer m_waves_vb;
	joj::D3D11IndexBuffer m_waves_ib;

	i32 m_box_vertex_offset = 0;
	u32 m_box_index_offset = 0;
	u32 m_box_index_count = 0;
	joj::D3D11VertexBuffer m_box_vb;
	joj::D3D11IndexBuffer m_box_ib;

	joj::D3D11VertexBuffer m_screen_quad_vb;
	joj::D3D11IndexBuffer m_screen_quad_ib;

	ID3D11ShaderResourceView* m_grass_map_SRV = nullptr;
	ID3D11ShaderResourceView* m_waves_map_SRV = nullptr;
	ID3D11ShaderResourceView* m_crate_map_SRV = nullptr;

	ID3D11ShaderResourceView* m_offscreen_SRV;
	ID3D11UnorderedAccessView* m_offscreen_UAV;
	ID3D11RenderTargetView* m_offscreen_RTV;

	Waves m_waves{ 160, 160, 1.0f, 0.03f, 5.0f, 0.3f };
	
	joj::DirectionalLight m_dir_lights[3];
	joj::SimpleMaterial m_land_mat;
	joj::SimpleMaterial m_waves_mat;
	joj::SimpleMaterial m_box_mat;

	joj::JFloat4x4 m_grass_tex_transform = joj::float4x4_identity();
	joj::JFloat4x4 m_water_tex_transform = joj::float4x4_identity();
	joj::JFloat4x4 m_land_world = joj::float4x4_identity();
	joj::JFloat4x4 m_waves_world = joj::float4x4_identity();
	joj::JFloat4x4 m_box_world = joj::float4x4_identity();

	void build_render_states();

	u32 m_land_index_count = 0;
	void build_land_geometry_buffers();
	void build_waves_geometry_buffers();
	void build_crate_geometry_buffers();
	void build_screen_quad_geometry_buffers();
	void build_offscreen_views();

	ID3D11SamplerState* m_sampler_state = nullptr;
	void build_textures();

	joj::D3D11Shader m_vert_shader;
	joj::D3D11Shader m_horz_shader;
	void build_shaders();

	std::vector<D3D11_INPUT_ELEMENT_DESC> m_input_desc;
	ID3D11InputLayout* m_input_layout = nullptr;
	ID3D11InputLayout* m_tree_input_layout = nullptr;
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
	f32 speed = 50.0f;
	b8 is_wireframe = false;

	joj::JFloat4x4 mView = joj::float4x4_identity();
	joj::JFloat4x4 mProj = joj::float4x4_identity();

	f64 prev = 0.0f;
	f32 t_base = 0.0f;
	joj::JFloat2 m_water_tex_offset;
	joj::RenderOptions m_render_options = joj::RenderOptions::Lighting;
	i32 m_light_count = 2;
	joj::D3D11RenderState m_render_state;
	joj::D3D11BlurFilter m_blur;

	u32 m_tree_count = 16;
	b8 m_alpha_to_converage_on = false;

	void draw_wapper();
	void draw_screen_quad();
};

// ---------------------------------------------------------------------------------

#endif // BLUR_DEMO_H