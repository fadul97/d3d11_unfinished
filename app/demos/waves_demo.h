#ifndef WAVES_DEMO_H
#define WAVES_DEMO_H

// ---------------------------------------------------------------------------------

#include "joj/application/app.h"
#include "joj/renderer/d3d11/shader_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include <renderer/d3d11/index_buffer_d3d11.h>
#include <renderer/d3d11/constant_buffer_d3d11.h>
#include "joj/math/jmath.h"
#include "joj/systems/camera/free_camera.h"
#include "joj/renderer/d3d11/scene_d3d11.h"
#include "waves.h"

// ---------------------------------------------------------------------------------

struct Vertex
{
	joj::JFloat3 Pos;
	joj::JFloat4 Color;
};

// ---------------------------------------------------------------------------------

class WavesDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

public:
	joj::JFloat4x4 m_grid_world = joj::float4x4_identity();
	joj::JFloat4x4 m_waves_world = joj::float4x4_identity();

	Waves m_waves{ 200, 200, 0.8f, 0.03f, 3.25f, 0.4f };
	
	joj::D3D11VertexBuffer m_land_vb;
	joj::D3D11IndexBuffer m_land_ib;

	joj::D3D11VertexBuffer m_waves_vb;
	joj::D3D11IndexBuffer m_waves_ib;
	
	u32 m_grid_index_count = 0;
	void build_land_geometry_buffers();
	void build_waves_geometry_buffers();

	joj::D3D11Shader m_shader;
	void build_shaders();

	std::vector<D3D11_INPUT_ELEMENT_DESC> m_input_desc;
	ID3D11InputLayout* m_input_layout = nullptr;
	void build_vertex_layout();

	joj::D3D11ConstantBuffer m_cb;
	void build_constant_buffer();

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

	f64 prev = 0.0f;
	f32 t_base = 0.0f;
};

// ---------------------------------------------------------------------------------

#endif // WAVES_DEMO_H