#ifndef SHAPES_DEMO_H
#define SHAPES_DEMO_H

// ---------------------------------------------------------------------------------

#include "joj/application/app.h"
#include <DirectXMath.h>
#include "joj/renderer/d3d11/3d/renderer_d3d11.h"
#include <unordered_map>
#include <string>
#include "joj/renderer/d3d11/3d/shader_d3d11.h"
#include "joj/renderer/d3d11/3d/vertex_buffer_d3d11.h"
#include <renderer/d3d11/3d/index_buffer_d3d11.h>
#include <renderer/d3d11/3d/constant_buffer_d3d11.h>
#include "joj/math/jmath.h"
#include "joj/resources/geometry/cube.h"
#include <resources/geometry/sphere.h>
#include <resources/geometry/cylinder.h>
#include <resources/geometry/geosphere.h>
#include <resources/geometry/grid.h>
#include <resources/geometry/quad.h>
#include "joj/systems/camera/free_camera.h"
#include <memory>
#include "joj/renderer/d3d11/3d/renderable_object_d3d11.h"
#include "joj/renderer/d3d11/3d/scene_d3d11.h"

// ---------------------------------------------------------------------------------

struct Vertex
{
	joj::JFloat3 Pos;
	joj::JFloat4 Color;
};

// ---------------------------------------------------------------------------------

class ShapesDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

public:
	joj::JFloat4x4 m_box_world = joj::float4x4_identity();
	joj::JFloat4x4 m_grid_world = joj::float4x4_identity();
	joj::JFloat4x4 m_sphere_world[10];
	joj::JFloat4x4 m_cyl_world[10];

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

	joj::D3D11Scene m_scene;
	joj::D3D11VertexBuffer m_vb;
	joj::D3D11IndexBuffer m_ib;
	void build_geometry_buffers();

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
};

// ---------------------------------------------------------------------------------

#endif // SHAPES_DEMO_H