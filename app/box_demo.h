#ifndef BOX_DEMO_H
#define BOX_DEMO_H

#include "joj/defines.h"

#if JPLATFORM_WINDOWS

#include "joj/application/app.h"
#include <DirectXMath.h>
#include "joj/renderer/d3d11/renderer_d3d11.h"
#include <unordered_map>
#include <string>
#include "joj/renderer/d3d11/shader_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include <renderer/d3d11/index_buffer_d3d11.h>
#include <renderer/d3d11/constant_buffer_d3d11.h>
#include "joj/math/jmath.h"
#include "joj/resources/geometry/cube.h"

struct Vertex
{
	joj::JFloat3 Pos;
	joj::JFloat4 Color;
};

struct ObjectConstants
{
	joj::JFloat4x4 World = joj::float4x4_identity();
};

template <typename T>
static T Clamp(const T& x, const T& low, const T& high)
{
	return x < low ? low : (x > high ? high : x);
}

class BoxDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

public:
	joj::Cube cube{ 3.0f, 3.0f, 3.0f };
	joj::D3D11VertexBuffer m_vb;
	joj::D3D11IndexBuffer m_ib;
	void build_geometry_buffers(joj::D3D11Renderer& renderer);

	joj::D3D11Shader m_shader;
	void build_shaders(joj::D3D11Renderer& renderer);

	std::vector<D3D11_INPUT_ELEMENT_DESC> m_input_desc;
	ID3D11InputLayout* m_input_layout = nullptr;
	void build_vertex_layout(joj::D3D11Renderer& renderer);

	joj::D3D11ConstantBuffer m_cb;
	void build_constant_buffer(joj::D3D11Renderer& renderer);

	joj::JFloat4x4 mWorld = joj::float4x4_identity();
	joj::JFloat4x4 mView = joj::float4x4_identity();
	joj::JFloat4x4 mProj = joj::float4x4_identity();
};

#endif // JPLATFORM_WINDOWS

#endif // BOX_DEMO_H