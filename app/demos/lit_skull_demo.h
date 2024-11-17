#ifndef LIT_SKULL_DEMO_H
#define LIT_SKULL_DEMO_H

// ---------------------------------------------------------------------------------

#include "joj/application/app.h"
#include "joj/math/jmath.h"
#include "joj/renderer/d3d11/shader_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include <renderer/d3d11/index_buffer_d3d11.h>
#include <renderer/d3d11/constant_buffer_d3d11.h>
#include "joj/systems/camera/free_camera.h"
#include "joj/systems/light/light.h"
#include "joj/systems/material/material.h"
#include <vector>

// ---------------------------------------------------------------------------------

class LitSkullDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

public:
	joj::JFloat4x4 m_grid_world = joj::float4x4_identity();
	joj::JFloat4x4 m_box_world = joj::float4x4_identity();
	joj::JFloat4x4 m_skull_world = joj::float4x4_identity();
	joj::JFloat4x4 m_sphere_world[10];
	joj::JFloat4x4 m_cyl_world[10];

	joj::DirectionalLight m_dir_lights[3];
	joj::SimpleMaterial m_grid_mat;
	joj::SimpleMaterial m_cylinder_mat;
	joj::SimpleMaterial m_sphere_mat;
	joj::SimpleMaterial m_box_mat;
	joj::SimpleMaterial m_skull_mat;

	i32 m_box_vertex_offset = 0;
	i32 m_grid_vertex_offset = 0;
	i32 m_sphere_vertex_offset = 0;
	i32 m_cylinder_vertex_offset = 0;

	u32 m_box_index_count = 0;
	u32 m_grid_index_count = 0;
	u32 m_sphere_index_count = 0;
	u32 m_cylinder_index_count = 0;

	u32 m_box_index_offset = 0;
	u32 m_grid_index_offset = 0;
	u32 m_sphere_index_offset = 0;
	u32 m_cylinder_index_offset = 0;

	joj::D3D11VertexBuffer m_shapes_vb;
	joj::D3D11IndexBuffer m_shapes_ib;
	void build_shapes_geometry_buffers();

	u32 m_skull_index_count = 0;
	joj::D3D11VertexBuffer m_skull_vb;
	joj::D3D11IndexBuffer m_skull_ib;
	void build_skull_geometry_buffer();

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

	u32 m_light_count = 3;
};

// ---------------------------------------------------------------------------------

#endif // LIT_SKULL_DEMO_H