#ifndef CRATE_DEMO_H
#define CRATE_DEMO_H

#include "joj/application/app.h"

#if JPLATFORM_WINDOWS

// ---------------------------------------------------------------------------------

#include "joj/renderer/d3d11/shader_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include <renderer/d3d11/index_buffer_d3d11.h>
#include <renderer/d3d11/constant_buffer_d3d11.h>
#include "joj/math/jmath.h"
#include "joj/resources/geometry/cube.h"
#include "joj/systems/camera/free_camera.h"
#include <systems/light/light.h>
#include <systems/material/material.h>

// ---------------------------------------------------------------------------------

class CrateDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

public:
	joj::JFloat4x4 m_box_world = joj::float4x4_identity();
	joj::JFloat4x4 m_tex_transform = joj::float4x4_identity();

	joj::DirectionalLight m_dir_lights[2];
	joj::SimpleMaterial m_box_mat;

	ID3D11ShaderResourceView* m_diffuse_map_SRV = nullptr;
	ID3D11SamplerState* m_sampler_state = nullptr;
	void build_texture();

	i32 m_box_vertex_offset = 0;
	u32 m_box_index_offset = 0;
	u32 m_box_index_count = 0;
	joj::D3D11VertexBuffer m_vb;
	joj::D3D11IndexBuffer m_ib;
	void build_geometry_buffers();

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
	f32 speed = 20.0f;
	b8 is_wireframe = false;

	joj::JFloat4x4 mView = joj::float4x4_identity();
	joj::JFloat4x4 mProj = joj::float4x4_identity();

	i32 m_light_count = 2;
	b32 m_use_texture = 1;
};

// ---------------------------------------------------------------------------------

#endif // JPLATFORM_WINDOWS

#endif // CRATE_DEMO_H