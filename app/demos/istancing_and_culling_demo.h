#ifndef ISTANCING_AND_CULLING_DEMO_H
#define ISTANCING_AND_CULLING_DEMO_H

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

// ---------------------------------------------------------------------------------

struct InstancedData
{
	joj::JFloat4x4 world;
	joj::JFloat4 color;
};

// ---------------------------------------------------------------------------------

class IstancingAndCullingDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

	void build_shaders();
	void build_vertex_layout();
	void build_geometry_buffers();
	void build_instanced_buffer();
	void build_constant_buffers();

public:
	joj::D3D11VertexBuffer m_skull_vb;
	joj::D3D11IndexBuffer m_skull_ib;
	joj::D3D11VertexBuffer m_instanced_buffer;

	// Bounding box of the skull
	DirectX::BoundingBox m_skull_box;
	DirectX::BoundingFrustum m_cam_frustum;

	u32 m_visible_object_count = 0;

	// Keep a system memory copy of the world matrices for culling
	std::vector<InstancedData> m_instanced_data;

	b8 m_frustum_culling_enabled = false;

	joj::DirectionalLight m_dir_lights[3];
	joj::SimpleMaterial m_skull_mat;

	joj::JFloat4x4 m_skull_world = joj::float4x4_identity();

	u32 m_skull_index_count = 0;

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
};

// ---------------------------------------------------------------------------------

#endif // ISTANCING_AND_CULLING_DEMO_H