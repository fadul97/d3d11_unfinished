#ifndef TREE_BILLBOARD_DEMO_H
#define TREE_BILLBOARD_DEMO_H

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

// ---------------------------------------------------------------------------------

enum class RenderOptions
{
	Lighting,
	Textures,
	TexturesAndFog
};

// ---------------------------------------------------------------------------------

class TreeBillBoarddDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

public:
	joj::JFloat4x4 m_land_world = joj::float4x4_identity();
	joj::JFloat4x4 m_waves_world = joj::float4x4_identity();
	joj::JFloat4x4 m_grass_tex_transform = joj::float4x4_identity();
	joj::JFloat4x4 m_water_tex_transform = joj::float4x4_identity();
	joj::JFloat4x4 m_box_world = joj::float4x4_identity();

	joj::DirectionalLight m_dir_lights[3];
	joj::SimpleMaterial m_land_mat;
	joj::SimpleMaterial m_waves_mat;
	joj::SimpleMaterial m_box_mat;
	joj::SimpleMaterial m_tree_mat;

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

	Waves m_waves{ 160, 160, 1.0f, 0.03f, 5.0f, 0.3f };
	
	joj::D3D11VertexBuffer m_land_vb;
	joj::D3D11IndexBuffer m_land_ib;

	joj::D3D11VertexBuffer m_waves_vb;
	joj::D3D11IndexBuffer m_waves_ib;

	i32 m_box_vertex_offset = 0;
	u32 m_box_index_offset = 0;
	u32 m_box_index_count = 0;
	joj::D3D11VertexBuffer m_box_vb;
	joj::D3D11IndexBuffer m_box_ib;

	joj::D3D11VertexBuffer m_tree_sprites_vb;
	
	u32 m_land_index_count = 0;
	void build_land_geometry_buffers();
	void build_waves_geometry_buffers();
	void build_crate_geometry_buffers();
	void build_tree_sprites_buffers();

	ID3D11ShaderResourceView* m_grass_map_SRV = nullptr;
	ID3D11ShaderResourceView* m_waves_map_SRV = nullptr;
	ID3D11ShaderResourceView* m_box_map_SRV = nullptr;
	ID3D11Texture2D* m_tree_texture_array = nullptr;
	ID3D11ShaderResourceView* m_tree_texture_map_array_SRV = nullptr;
	ID3D11SamplerState* m_sampler_state = nullptr;
	void build_textures();
	void build_texture_array();

	joj::D3D11Shader m_basic_shader;
	joj::D3D11Shader m_tree_sprite_shader;
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
	RenderOptions m_render_options = RenderOptions::Lighting;
	i32 m_light_count = 2;

	u32 m_tree_count = 16;
	b8 m_alpha_to_converage_on = false;
	void draw_tree_sprites();
};

// ---------------------------------------------------------------------------------

#endif // TREE_BILLBOARD_DEMO_H