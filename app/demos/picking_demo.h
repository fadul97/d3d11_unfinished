#ifndef PICKING_DEMO_H
#define PICKING_DEMO_H

// ---------------------------------------------------------------------------------

#include "joj/application/app.h"
#include "joj/renderer/d3d11/shader_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include <renderer/d3d11/index_buffer_d3d11.h>
#include <renderer/d3d11/constant_buffer_d3d11.h>
#include "joj/math/jmath.h"
#include "joj/systems/camera/free_camera.h"
#include <vector>
#include <DirectXCollision.h>
#include <systems/light/light.h>
#include <systems/material/material.h>
#include <renderer/d3d11/render_state_d3d11.h>

// ---------------------------------------------------------------------------------

struct Basic32
{
	joj::JFloat3 pos;
	joj::JFloat3 normal;
	joj::JFloat2 tex;
};

// ---------------------------------------------------------------------------------

class PickingDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

	void build_shaders();
	void build_vertex_layout();
	void build_render_states();
	void build_geometry_buffers();
	void build_constant_buffers();
	void pick(i32 sx, i32 sy);

public:
	joj::D3D11VertexBuffer m_mesh_vb;
	joj::D3D11IndexBuffer m_mesh_ib;

	// Keep a system memory copy of the world matrices for culling
	std::vector<Basic32> m_mesh_vertices;
	std::vector<u32> m_mesh_indices;

	// Bounding box of the mesh
	DirectX::BoundingBox m_mesh_box;

	joj::DirectionalLight m_dir_lights[3];
	joj::SimpleMaterial m_mesh_mat;
	joj::SimpleMaterial m_picked_triangle_mat;

	// Define transformations from local spaces to world space
	joj::JFloat4x4 m_mesh_world = joj::float4x4_identity();

	u32 m_mesh_index_count = 0;
	u32 m_picked_triangle = -1;

	joj::D3D11Shader m_shader;

	std::vector<D3D11_INPUT_ELEMENT_DESC> m_input_desc;
	ID3D11InputLayout* m_input_layout = nullptr;

	joj::D3D11ConstantBuffer m_object_cb;
	joj::D3D11ConstantBuffer m_frame_cb;

	ID3D11SamplerState* m_sampler_state = nullptr;

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
	joj::D3D11RenderState m_render_state;
};

// ---------------------------------------------------------------------------------

#endif // PICKING_DEMO_H