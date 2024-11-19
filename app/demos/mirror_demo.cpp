#include "mirror_demo.h"

#if JPLATFORM_WINDOWS

// ---------------------------------------------------------------------------------

#include "logger.h"
#include <DirectXColors.h>
#include <d3d11.h>
#include <string>
#include <d3dcompiler.h>
#include "joj/engine.h"
#include <fstream>
#include <string>
#include <resources/geometry/grid.h>
#include "joj/renderer/d3d11/scene_d3d11.h" // for ObjectCB
#include "joj/systems/light/light_debug.h"
#include <renderer/d3d11/DDSTextureLoader11.h>
#include <resources/geometry/cube.h>

// ---------------------------------------------------------------------------------

f32 get_height(f32 x, f32 z)
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

f32 RandF()
{
	return (f32)(rand()) / (f32)RAND_MAX;
}

// Returns random f32 in [a, b).
f32 RandF(f32 a, f32 b)
{
	return a + RandF() * (b - a);
}

i32 Rand(i32 a, i32 b)
{
	return a + rand() % ((b - a) + 1);
}

f32 get_hill_height(f32 x, f32 z)
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

joj::JFloat3 get_hill_normal(f32 x, f32 z)
{
	// n = (-df/dx, 1, -df/dz)
	joj::JFloat3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	joj::JVector4 unit_normal = DirectX::XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unit_normal);

	return n;
}

template<typename T>
static T Max(const T& a, const T& b)
{
	return a > b ? a : b;
}

// ---------------------------------------------------------------------------------

void MirrorDemo::init()
{
	// Basic Initialization
	{
		// Hide camera
		ShowCursor(false);

		// FIXME: Stop using magical numbers
		// Locking mouse at the center of window
		centerX += 1100;
		centerY += 400;

		// Projection Matrix
		constexpr f32 fov_angle = 45;
		DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(fov_angle), 800.0f / 600.0f, 0.1f, 1000.0f
		);
		DirectX::XMStoreFloat4x4(&mProj, P);

		// Setup Mouse movement speed
		camera.m_movement_speed = 2.0f;
	}

	// ---------------------------------------------------
	// Initialize Matrixes
	// ---------------------------------------------------

	joj::JMatrix4x4 I = joj::matrix4x4_identity();
	XMStoreFloat4x4(&m_room_world, I);
	XMStoreFloat4x4(&m_skull_world, I);
	XMStoreFloat4x4(&mView, I);

	// ---------------------------------------------------
	// Initialize Lights
	// ---------------------------------------------------

	// Directional light.
	m_dir_lights[0].ambient = joj::JFloat4(0.2f, 0.2f, 0.2f, 1.0f);
	m_dir_lights[0].diffuse = joj::JFloat4(0.5f, 0.5f, 0.5f, 1.0f);
	m_dir_lights[0].specular = joj::JFloat4(0.5f, 0.5f, 0.5f, 1.0f);
	m_dir_lights[0].direction = joj::JFloat3(0.57735f, -0.57735f, 0.57735f);

	m_dir_lights[1].ambient = joj::JFloat4(0.0f, 0.0f, 0.0f, 1.0f);
	m_dir_lights[1].diffuse = joj::JFloat4(0.20f, 0.20f, 0.20f, 1.0f);
	m_dir_lights[1].specular = joj::JFloat4(0.25f, 0.25f, 0.25f, 1.0f);
	m_dir_lights[1].direction = joj::JFloat3(-0.57735f, -0.57735f, 0.57735f);

	m_dir_lights[2].ambient = joj::JFloat4(0.0f, 0.0f, 0.0f, 1.0f);
	m_dir_lights[2].diffuse = joj::JFloat4(0.2f, 0.2f, 0.2f, 1.0f);
	m_dir_lights[2].specular = joj::JFloat4(0.0f, 0.0f, 0.0f, 1.0f);
	m_dir_lights[2].direction = joj::JFloat3(0.0f, -0.707f, -0.707f);

	// ---------------------------------------------------
	// Initialize Materials
	// ---------------------------------------------------

	m_room_mat.ambient = joj::JFloat4(0.5f, 0.5f, 0.5f, 1.0f);
	m_room_mat.diffuse = joj::JFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	m_room_mat.specular = joj::JFloat4(0.4f, 0.4f, 0.4f, 16.0f);

	m_skull_mat.ambient = joj::JFloat4(0.5f, 0.5f, 0.5f, 1.0f);
	m_skull_mat.diffuse = joj::JFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	m_skull_mat.specular = joj::JFloat4(0.4f, 0.4f, 0.4f, 16.0f);

	// Reflected material is transparent so it blends into mirror.
	m_mirror_mat.ambient = joj::JFloat4(0.5f, 0.5f, 0.5f, 1.0f);
	m_mirror_mat.diffuse = joj::JFloat4(1.0f, 1.0f, 1.0f, 0.5f);
	m_mirror_mat.specular = joj::JFloat4(0.4f, 0.4f, 0.4f, 16.0f);

	m_shadow_mat.ambient = joj::JFloat4(0.0f, 0.0f, 0.0f, 1.0f);
	m_shadow_mat.diffuse = joj::JFloat4(0.0f, 0.0f, 0.0f, 0.5f);
	m_shadow_mat.specular = joj::JFloat4(0.0f, 0.0f, 0.0f, 16.0f);

	// ---------------------------------------------------
	// Build Resources
	// ---------------------------------------------------

	build_shaders();
	build_vertex_layout();
	build_render_states();
	build_textures();
	build_room_geometry_buffers();
	build_skull_geometry_buffers();
	build_constant_buffers();
}

// ---------------------------------------------------------------------------------

void MirrorDemo::build_render_states()
{
	// ---------------------------------------------------
	// Describe and Create Wireframe Rasterizer State
	// ---------------------------------------------------

	D3D11_RASTERIZER_DESC wireframe_desc;
	ZeroMemory(&wireframe_desc, sizeof(D3D11_RASTERIZER_DESC));
	wireframe_desc.FillMode = D3D11_FILL_WIREFRAME;
	wireframe_desc.CullMode = D3D11_CULL_BACK;
	wireframe_desc.FrontCounterClockwise = false;
	wireframe_desc.DepthClipEnable = true;

	// Create rasterizer state
	if (joj::Engine::s_renderer->get_device()->CreateRasterizerState(&wireframe_desc, &m_wireframe_RS) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create RasterizerState.");
	}

	// ---------------------------------------------------
	// Describe and Create NoCull Rasterizer State
	// ---------------------------------------------------

	D3D11_RASTERIZER_DESC no_cull_desc;
	ZeroMemory(&no_cull_desc, sizeof(D3D11_RASTERIZER_DESC));
	no_cull_desc.FillMode = D3D11_FILL_SOLID;
	no_cull_desc.CullMode = D3D11_CULL_NONE;
	no_cull_desc.FrontCounterClockwise = false;
	no_cull_desc.DepthClipEnable = true;

	// Create rasterizer state
	if (joj::Engine::s_renderer->get_device()->CreateRasterizerState(&no_cull_desc, &m_no_cull_RS) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create RasterizerState.");
	}

	// ---------------------------------------------------
	// Describe and Create CullClockWise Rasterizer State
	// ---------------------------------------------------

	D3D11_RASTERIZER_DESC cull_clock_wise_desc;
	ZeroMemory(&cull_clock_wise_desc, sizeof(D3D11_RASTERIZER_DESC));
	cull_clock_wise_desc.FillMode = D3D11_FILL_SOLID;
	cull_clock_wise_desc.CullMode = D3D11_CULL_BACK;
	cull_clock_wise_desc.FrontCounterClockwise = true;
	cull_clock_wise_desc.DepthClipEnable = true;

	// Create rasterizer state
	if (joj::Engine::s_renderer->get_device()->CreateRasterizerState(&cull_clock_wise_desc, &m_cull_clock_wise_RS) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create RasterizerState.");
	}

	// ---------------------------------------------------
	// Describe and Create AlphaToCoverage Blend State
	// ---------------------------------------------------

	D3D11_BLEND_DESC alpha_to_coverage_desc = { 0 };
	alpha_to_coverage_desc.AlphaToCoverageEnable = true;
	alpha_to_coverage_desc.IndependentBlendEnable = false;
	alpha_to_coverage_desc.RenderTarget[0].BlendEnable = false;
	alpha_to_coverage_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create blend state
	if (joj::Engine::s_renderer->get_device()->CreateBlendState(&alpha_to_coverage_desc, &m_alpha_to_coverage_BS) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Blend State.");
	}

	// ---------------------------------------------------
	// Describe and Create Transparent Blend State
	// ---------------------------------------------------

	D3D11_BLEND_DESC transparent_desc = { 0 };
	transparent_desc.AlphaToCoverageEnable = false;
	transparent_desc.IndependentBlendEnable = false;

	transparent_desc.RenderTarget[0].BlendEnable = true;
	transparent_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparent_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparent_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparent_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparent_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparent_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparent_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create blend state
	if (joj::Engine::s_renderer->get_device()->CreateBlendState(&transparent_desc, &m_transparent_BS) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Blend State.");
	}

	// ---------------------------------------------------
	// Describe and Create NoRenderTargetWrite Blend State
	// ---------------------------------------------------

	D3D11_BLEND_DESC no_render_target_write_desc = { 0 };
	no_render_target_write_desc.AlphaToCoverageEnable = false;
	no_render_target_write_desc.IndependentBlendEnable = false;

	no_render_target_write_desc.RenderTarget[0].BlendEnable = false;
	no_render_target_write_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	no_render_target_write_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	no_render_target_write_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	no_render_target_write_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	no_render_target_write_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	no_render_target_write_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	no_render_target_write_desc.RenderTarget[0].RenderTargetWriteMask = 0;

	// Create blend state
	if (joj::Engine::s_renderer->get_device()->CreateBlendState(&no_render_target_write_desc, &m_no_render_target_write_BS) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Blend State.");
	}

	// ---------------------------------------------------
	// Describe and Create MarkMirror Depth Stencil State
	// ---------------------------------------------------

	D3D11_DEPTH_STENCIL_DESC mark_mirror_desc = { 0 };
	mark_mirror_desc.DepthEnable = true;
	mark_mirror_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	mark_mirror_desc.DepthFunc = D3D11_COMPARISON_LESS;
	mark_mirror_desc.StencilEnable = true;
	mark_mirror_desc.StencilReadMask = 0xff;
	mark_mirror_desc.StencilWriteMask = 0xff;

	mark_mirror_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mark_mirror_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mark_mirror_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mark_mirror_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// We are not rendering backfacing polygons, so these settings do not matter.
	mark_mirror_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mark_mirror_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mark_mirror_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mark_mirror_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil
	if (joj::Engine::s_renderer->get_device()->CreateDepthStencilState(&mark_mirror_desc, &m_mark_mirror_DSS) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Depth Stencil State.");
	}

	// ---------------------------------------------------
	// Describe and Create DrawReflection Depth Stencil State
	// ---------------------------------------------------

	D3D11_DEPTH_STENCIL_DESC draw_reflection_desc = { 0 };
	draw_reflection_desc.DepthEnable = true;
	draw_reflection_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	draw_reflection_desc.DepthFunc = D3D11_COMPARISON_LESS;
	draw_reflection_desc.StencilEnable = true;
	draw_reflection_desc.StencilReadMask = 0xff;
	draw_reflection_desc.StencilWriteMask = 0xff;

	draw_reflection_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	// We are not rendering backfacing polygons, so these settings do not matter.
	draw_reflection_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	draw_reflection_desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	// Create depth stencil
	if (joj::Engine::s_renderer->get_device()->CreateDepthStencilState(&draw_reflection_desc, &m_draw_reflection_DSS) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Depth Stencil State.");
	}

	// ---------------------------------------------------
	// Describe and Create NoDoubleBlend Depth Stencil State
	// ---------------------------------------------------

	D3D11_DEPTH_STENCIL_DESC no_double_blend_desc = { 0 };
	no_double_blend_desc.DepthEnable = true;
	no_double_blend_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	no_double_blend_desc.DepthFunc = D3D11_COMPARISON_LESS;
	no_double_blend_desc.StencilEnable = true;
	no_double_blend_desc.StencilReadMask = 0xff;
	no_double_blend_desc.StencilWriteMask = 0xff;

	no_double_blend_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	no_double_blend_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	no_double_blend_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	no_double_blend_desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	// We are not rendering backfacing polygons, so these settings do not matter.
	no_double_blend_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	no_double_blend_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	no_double_blend_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	no_double_blend_desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	// Create depth stencil
	if (joj::Engine::s_renderer->get_device()->CreateDepthStencilState(&no_double_blend_desc, &m_no_double_blend_DSS) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Depth Stencil State.");
	}
}

// ---------------------------------------------------------------------------------

void MirrorDemo::build_textures()
{
	// ---------------------------------------------------
	// Create Floor DDS Texture
	// ---------------------------------------------------

	if (DirectX::CreateDDSTextureFromFile(
		joj::Engine::s_renderer->get_device(),
		L"../../../../app/textures/checkboard.dds",
		nullptr,
		&m_floor_diffuse_map_SRV
	) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create DDS Texture from file 'checkboard.dds'.");
	}
	else
	{
		JINFO("Created DDS Texture from file!");
	}

	// ---------------------------------------------------
	// Create Brick DDS Texture
	// ---------------------------------------------------

	if (DirectX::CreateDDSTextureFromFile(
		joj::Engine::s_renderer->get_device(),
		L"../../../../app/textures/brick01.dds",
		nullptr,
		&m_wall_diffuse_map_SRV
	) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create DDS Texture from file 'brick01.dds'.");
	}
	else
	{
		JINFO("Created DDS Texture from file!");
	}

	// ---------------------------------------------------
	// Create Ice DDS Texture
	// ---------------------------------------------------

	if (DirectX::CreateDDSTextureFromFile(
		joj::Engine::s_renderer->get_device(),
		L"../../../../app/textures/ice.dds",
		nullptr,
		&m_mirror_map_SRV
	) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create DDS Texture from file 'ice.dds'.");
	}
	else
	{
		JINFO("Created DDS Texture from file!");
	}

	// ---------------------------------------------------
	// Setup and Create Sampler State
	// ---------------------------------------------------

	// Describe Sampler State
	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.MaxAnisotropy = 4;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	if (joj::Engine::s_renderer->get_device()->CreateSamplerState(&sampler_desc, &m_sampler_state) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Sampler State.");
		return;
	}
}

// ---------------------------------------------------------------------------------

void MirrorDemo::build_room_geometry_buffers()
{
	// Create and specify geometry.  For this sample we draw a floor
	// and a wall with a mirror on it.  We put the floor, wall, and
	// mirror geometry in one vertex buffer.
	//
	//   |--------------|
	//   |              |
	//   |----|----|----|
	//   |Wall|Mirr|Wall|
	//   |    | or |    |
	//   /--------------/
	//  /   Floor      /
	// /--------------/

	joj::GeometryVertex v[30];

	// Basic32(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
	// GeometryVertex(f32 px, f32 py, f32 pz, f32 nx, f32 ny, f32 nz, f32 r, f32 g, f32 b, f32 a, f32 u, f32 v);
	v[0] = joj::GeometryVertex(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f);
	v[1] = joj::GeometryVertex(-3.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[2] = joj::GeometryVertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 4.0f, 0.0f);

	v[3] = joj::GeometryVertex(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f);
	v[4] = joj::GeometryVertex(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 4.0f, 0.0f);
	v[5] = joj::GeometryVertex(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 4.0f, 4.0f);

	// Wall: Observe we tile texture coordinates, and that we
	// leave a gap in the middle for the mirror.
	v[6] = joj::GeometryVertex(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f);
	v[7] = joj::GeometryVertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[8] = joj::GeometryVertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f);

	v[9] = joj::GeometryVertex(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f);
	v[10] = joj::GeometryVertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f);
	v[11] = joj::GeometryVertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, 2.0f);

	v[12] = joj::GeometryVertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f);
	v[13] = joj::GeometryVertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[14] = joj::GeometryVertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 2.0f, 0.0f);

	v[15] = joj::GeometryVertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f);
	v[16] = joj::GeometryVertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 2.0f, 0.0f);
	v[17] = joj::GeometryVertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 2.0f, 2.0f);

	v[18] = joj::GeometryVertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[19] = joj::GeometryVertex(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[20] = joj::GeometryVertex(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 6.0f, 0.0f);

	v[21] = joj::GeometryVertex(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[22] = joj::GeometryVertex(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 6.0f, 0.0f);
	v[23] = joj::GeometryVertex(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 6.0f, 1.0f);

	// Mirror
	v[24] = joj::GeometryVertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[25] = joj::GeometryVertex(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[26] = joj::GeometryVertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	v[27] = joj::GeometryVertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[28] = joj::GeometryVertex(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[29] = joj::GeometryVertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	// ---------------------------------------------------
	// Create Room Vertex Buffer
	// ---------------------------------------------------

	m_room_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * 30, v);

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_room_vb.get_buffer_desc(), m_room_vb.get_subdata(), &m_room_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void MirrorDemo::build_skull_geometry_buffers()
{
	// ---------------------------------------------------
	// Read model file
	// ---------------------------------------------------

	std::ifstream fin("../../../../app/models/skull.txt");
	if (!fin)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to find file 'skull.txt'.");
	}

	u32 vcount = 0;
	u32 tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	joj::JFloat4 black(0.0f, 0.0f, 0.0f, 1.0f);

	// ---------------------------------------------------
	// Setup vertices vector for model file
	// ---------------------------------------------------

	std::vector<joj::GeometryVertex> vertices(vcount);
	for (u32 i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].pos.x >> vertices[i].pos.y >> vertices[i].pos.z;
		fin >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;
		vertices[i].color = black; // Not using in Demo.hlsl shader file
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	// Create vertex buffer
	m_skull_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * vcount, vertices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_skull_vb.get_buffer_desc(), m_skull_vb.get_subdata(), &m_skull_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// ---------------------------------------------------
	// Setup Grid Index Count
	// ---------------------------------------------------

	m_skull_index_count = 3 * tcount;

	// ---------------------------------------------------
	// Setup indices vector for model file
	// ---------------------------------------------------

	std::vector<u32> indices(m_skull_index_count);
	for (u32 i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	// Create the index buffer
	m_skull_ib.setup(sizeof(u32) * m_skull_index_count, indices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_skull_ib.get_buffer_desc(), m_skull_ib.get_subdata(), &m_skull_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void MirrorDemo::build_shaders()
{
	// FIXME: Path is wrong
	m_shader.compile_vertex_shader(L"../../../../app/shaders/Demo.hlsl", "VS", "vs_5_0");
	m_shader.compile_pixel_shader(L"../../../../app/shaders/Demo.hlsl", "PS", "ps_5_0");

	joj::Engine::s_renderer->get_device()->CreateVertexShader(
		// A pointer to the compiled shader
		m_shader.get_vsblob()->GetBufferPointer(),
		// Size of the compiled vertex shader
		m_shader.get_vsblob()->GetBufferSize(),
		// A pointer to a class linkage interface
		nullptr,
		// Address of a pointer to a ID3D11VertexShader interface
		&m_shader.get_vertex_shader());

	joj::Engine::s_renderer->get_device()->CreatePixelShader(
		// A pointer to the compiled shader
		m_shader.get_psblob()->GetBufferPointer(),
		// Size of the compiled vertex shader
		m_shader.get_psblob()->GetBufferSize(),
		// A pointer to a class linkage interface
		nullptr,
		// Address of a pointer to a ID3D11VertexShader interface
		&m_shader.get_pixel_shader());
}

// ---------------------------------------------------------------------------------

void MirrorDemo::build_vertex_layout()
{
	// Create the vertex input layout.
	m_input_desc =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (joj::Engine::s_renderer->get_device()->CreateInputLayout(
		m_input_desc.data(),
		(u32)m_input_desc.size(),
		m_shader.get_vsblob()->GetBufferPointer(),
		m_shader.get_vsblob()->GetBufferSize(),
		&m_input_layout
	) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Input Layout.");
	}
}

// ---------------------------------------------------------------------------------

void MirrorDemo::build_constant_buffers()
{
	// ---------------------------------------------------
	// Setup PerObject Constant Buffer
	// ---------------------------------------------------

	m_object_cb.setup(joj::calculate_cb_byte_size(sizeof(joj::CBPerObject)), nullptr);

	// Create constant buffer
	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_object_cb.get_buffer_desc(), nullptr, &m_object_cb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Constant Buffer.");
	}

	// ---------------------------------------------------
	// Setup PerFrame Constant Buffer
	// ---------------------------------------------------

	m_frame_cb.setup(joj::calculate_cb_byte_size(sizeof(joj::CBPerFrame)), nullptr);

	// Create constant buffer
	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_frame_cb.get_buffer_desc(), nullptr, &m_frame_cb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Constant Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void MirrorDemo::update(const f32 dt)
{
	// Basic Input for LearningApp projects
	{
		// Exit with ESCAPE key
		if (joj::Engine::s_input->is_key_pressed(joj::KEY_ESCAPE))
			joj::Engine::close();

		if (joj::Engine::s_input->is_key_pressed(joj::KEY_TAB))
		{
			firstPerson = !firstPerson;
			hideCursor = !hideCursor;
			ShowCursor(hideCursor);
		}

		if (firstPerson)
		{
			// Now read the mouse position
			POINT cursorPos;
			GetCursorPos(&cursorPos);  // Get the current cursor position
			// Rotate freely inside window
			SetCursorPos(centerX, centerY);

			// Calculate the mouse movement
			i32 xoffset = cursorPos.x - centerX;
			i32 yoffset = centerY - cursorPos.y;

			//mouse_callback(JojEngine::Engine::pm->get_xmouse(), JojEngine::Engine::pm->get_ymouse());
			camera.process_mouse_movement(xoffset, yoffset);
		}

		// Change Rasterizer State
		if (joj::Engine::s_input->is_key_pressed('C'))
			is_wireframe = !is_wireframe;

		// Change Mouse Movement Speed state
		if (joj::Engine::s_input->is_key_pressed('F'))
			fast = !fast;

		if (fast)
			speed = 50.0f;
		else
			speed = 1.0f;

		if (joj::Engine::s_input->is_key_down('W'))
			camera.process_keyboard(joj::CameraMovement::FORWARD, dt * speed);

		if (joj::Engine::s_input->is_key_down(joj::KEY_S))
			camera.process_keyboard(joj::CameraMovement::BACKWARD, dt * speed);

		if (joj::Engine::s_input->is_key_down(joj::KEY_A))
			camera.process_keyboard(joj::CameraMovement::LEFT, dt * speed);

		if (joj::Engine::s_input->is_key_down(joj::KEY_D))
			camera.process_keyboard(joj::CameraMovement::RIGHT, dt * speed);

		DirectX::XMMATRIX V = camera.get_view_mat();
		XMStoreFloat4x4(&mView, V);
	}

	// ---------------------------------------------------
	// Read Demo Input
	// ---------------------------------------------------
	{
		if (joj::Engine::s_input->is_key_pressed('1'))
		{
			m_render_options = RenderOptions::Lighting;
			JDEBUG("Lighting");
		}

		if (joj::Engine::s_input->is_key_pressed('2'))
		{
			m_render_options = RenderOptions::Textures;
			JDEBUG("Textures");
		}

		if (joj::Engine::s_input->is_key_pressed('3'))
		{
			m_render_options = RenderOptions::TexturesAndFog;
			JDEBUG("TexturesAndFog");
		}

		// Increment Light count
		if (joj::Engine::s_input->is_key_pressed('L'))
			m_light_count = (m_light_count + 1) % 3;

		if (joj::Engine::s_input->is_key_pressed(joj::KEY_LEFT))
			m_skull_translation.x -= 1.0f * dt;

		if (joj::Engine::s_input->is_key_pressed(joj::KEY_RIGHT))
			m_skull_translation.x += 1.0f * dt;

		if (joj::Engine::s_input->is_key_pressed('K'))
			m_skull_translation.y += 1.0f * dt;

		if (joj::Engine::s_input->is_key_pressed('J'))
			m_skull_translation.y -= 1.0f * dt;
	}

	// Don't let user move below ground plane.
	m_skull_translation.y = Max(m_skull_translation.y, 0.0f);

	// Update the new world matrix.
	joj::JMatrix4x4 skull_rotate = DirectX::XMMatrixRotationY(0.5f * J_PI);
	joj::JMatrix4x4 skull_scale = DirectX::XMMatrixScaling(0.45f, 0.45f, 0.45f);
	joj::JMatrix4x4 skull_offset = DirectX::XMMatrixTranslation(m_skull_translation.x, m_skull_translation.y, m_skull_translation.z);
	joj::JMatrix4x4 combined = skull_rotate * skull_scale * skull_offset;
	XMStoreFloat4x4(&m_skull_world, combined);
}

// ---------------------------------------------------------------------------------

void MirrorDemo::draw()
{
	// Color fog and clear color should be the same so it can actually look like a fog
	const joj::JFloat4 fog_color{ 0.0f, 0.0f, 0.0f, 1.0f };

	// Default draw calls for every object
	{
		joj::Engine::s_renderer->clear(fog_color.x, fog_color.y, fog_color.z, fog_color.w);

		joj::Engine::s_renderer->get_device_context()->IASetInputLayout(m_input_layout);
		joj::Engine::s_renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		joj::Engine::s_renderer->get_device_context()->VSSetShader(m_shader.get_vertex_shader(), nullptr, 0u);
		joj::Engine::s_renderer->get_device_context()->PSSetShader(m_shader.get_pixel_shader(), nullptr, 0u);

		joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
		joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

		joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
		joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

		joj::Engine::s_renderer->get_device_context()->PSSetSamplers(0, 1, &m_sampler_state);
	}

	// Set constants and Update CBPerFrame
	joj::JMatrix4x4 view = DirectX::XMLoadFloat4x4(&mView);
	joj::JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&mProj);
	{
		// ---------------------------------------------------
		// Set Per Frame Constants
		// ---------------------------------------------------
		joj::CBPerFrame frame_cb;
		frame_cb.dir_lights[0] = m_dir_lights[0];
		frame_cb.dir_lights[1] = m_dir_lights[1];
		frame_cb.dir_lights[2] = m_dir_lights[2];
		frame_cb.eye_posw = camera.m_position;
		frame_cb.fog_start = 2.0f;
		frame_cb.fog_range = 40.0f;
		frame_cb.fog_color = fog_color;
		frame_cb.light_count = m_light_count;
		m_frame_cb.update(joj::Engine::s_renderer->get_device_context(), frame_cb);
	}

	i32 room_use_texture = 0;
	i32 skull_use_texture = 0;

	i32 room_alpha_clip = 0;
	i32 skull_alpha_clip = 0;

	i32 room_fog_enabled = 0;
	i32 skull_fog_enabled = 0;

	switch (m_render_options)
	{
	case RenderOptions::Lighting:
		room_use_texture = 0;
		room_alpha_clip = 0;
		room_fog_enabled = 0;
		
		skull_use_texture = 0;
		skull_alpha_clip = 0;
		skull_fog_enabled = 0;
		break;
	case RenderOptions::Textures:
		room_use_texture = 1;
		room_alpha_clip = 0;
		room_fog_enabled = 0;
		
		skull_use_texture = 0;
		skull_alpha_clip = 0;
		skull_fog_enabled = 0;
		break;
	case RenderOptions::TexturesAndFog:
		room_use_texture = 1;
		room_alpha_clip = 0;
		room_fog_enabled = 1;
		
		skull_use_texture = 0;
		skull_alpha_clip = 0;
		skull_fog_enabled = 1;
		break;
	default:
		break;
	}

	f32 blend_factor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	u32 stride = sizeof(joj::GeometryVertex);
	u32 offset = 0;

	// ---------------------------------------------------
	// Draw Floors and Walls to the back buffer as usual
	// ---------------------------------------------------
	{
		joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_room_vb.get_buffer(), &stride, &offset);

		joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_room_world);
		joj::JVector4 world_determinant = XMMatrixDeterminant(world);
		joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
		joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

		joj::JMatrix4x4 wvp = world * view * proj;

		joj::JMatrix4x4 I = joj::matrix4x4_identity();

		joj::CBPerObject room_cb;
		XMStoreFloat4x4(&room_cb.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&room_cb.world_inv_transpose, world_inv_transpose);
		XMStoreFloat4x4(&room_cb.wvp, XMMatrixTranspose(wvp));
		XMStoreFloat4x4(&room_cb.tex_transform, I);
		room_cb.material = m_room_mat;
		room_cb.use_texture = room_use_texture;
		room_cb.alpha_clip = room_alpha_clip;
		room_cb.fog_enabled = room_fog_enabled;
		m_object_cb.update(joj::Engine::s_renderer->get_device_context(), room_cb);

		// Draw Floor
		joj::Engine::s_renderer->get_device_context()->PSSetShaderResources(0, 1, &m_floor_diffuse_map_SRV);
		joj::Engine::s_renderer->get_device_context()->Draw(6, 0);

		// Draw Wall
		joj::Engine::s_renderer->get_device_context()->PSSetShaderResources(0, 1, &m_wall_diffuse_map_SRV);
		joj::Engine::s_renderer->get_device_context()->Draw(18, 6);
	}

	// ---------------------------------------------------
	// Draw Skull to the back buffer as usual
	// ---------------------------------------------------
	{
		joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_skull_vb.get_buffer(), &stride, &offset);
		joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_skull_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

		joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_skull_world);
		joj::JVector4 world_determinant = XMMatrixDeterminant(world);
		joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
		joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

		joj::JMatrix4x4 wvp = world * view * proj;

		joj::JMatrix4x4 I = joj::matrix4x4_identity();

		joj::CBPerObject skull_cb;
		XMStoreFloat4x4(&skull_cb.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&skull_cb.world_inv_transpose, world_inv_transpose);
		XMStoreFloat4x4(&skull_cb.wvp, XMMatrixTranspose(wvp));
		XMStoreFloat4x4(&skull_cb.tex_transform, I);
		skull_cb.material = m_skull_mat;
		skull_cb.use_texture = skull_use_texture;
		skull_cb.alpha_clip = skull_alpha_clip;
		skull_cb.fog_enabled = skull_fog_enabled;
		m_object_cb.update(joj::Engine::s_renderer->get_device_context(), skull_cb);

		// Draw Skull
		joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_skull_index_count, 0, 0);
	}

	// ---------------------------------------------------
	// Draw Mirror to Stencil Buffer only
	// ---------------------------------------------------
	{
		joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_room_vb.get_buffer(), &stride, &offset);

		joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_room_world);
		joj::JVector4 world_determinant = XMMatrixDeterminant(world);
		joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
		joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

		joj::JMatrix4x4 wvp = world * view * proj;

		joj::JMatrix4x4 I = joj::matrix4x4_identity();

		joj::CBPerObject mirror_cb;
		XMStoreFloat4x4(&mirror_cb.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&mirror_cb.world_inv_transpose, world_inv_transpose);
		XMStoreFloat4x4(&mirror_cb.wvp, XMMatrixTranspose(wvp));
		XMStoreFloat4x4(&mirror_cb.tex_transform, I);
		mirror_cb.material = m_mirror_mat;
		mirror_cb.use_texture = room_use_texture;
		mirror_cb.alpha_clip = room_alpha_clip;
		mirror_cb.fog_enabled = room_fog_enabled;
		m_object_cb.update(joj::Engine::s_renderer->get_device_context(), mirror_cb);

		// Do not write to render target.
		joj::Engine::s_renderer->get_device_context()->OMSetBlendState(m_no_render_target_write_BS, blend_factor, 0xffffffff);

		// Render visible mirror pixels to stencil buffer.
		// Do not write mirror depth to depth buffer at this point, otherwise it will occlude the reflection.
		joj::Engine::s_renderer->get_device_context()->OMSetDepthStencilState(m_mark_mirror_DSS, 1);

		// Draw Mirror
		joj::Engine::s_renderer->get_device_context()->Draw(6, 24);

		// Restore states.
		joj::Engine::s_renderer->get_device_context()->OMSetDepthStencilState(nullptr, 0);
		joj::Engine::s_renderer->get_device_context()->OMSetBlendState(nullptr, blend_factor, 0xffffffff);
	}

	// ---------------------------------------------------
	// Draw Skull Reflection
	// ---------------------------------------------------
	{
		joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_skull_vb.get_buffer(), &stride, &offset);
		joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_skull_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

		joj::JVector4 mirror_plane = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
		joj::JMatrix4x4 R = DirectX::XMMatrixReflect(mirror_plane);
		joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_skull_world) * R;
		joj::JVector4 world_determinant = XMMatrixDeterminant(world);
		joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
		joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

		joj::JMatrix4x4 wvp = world * view * proj;

		joj::JMatrix4x4 I = joj::matrix4x4_identity();

		joj::CBPerObject skull_reflection_cb;
		XMStoreFloat4x4(&skull_reflection_cb.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&skull_reflection_cb.world_inv_transpose, world_inv_transpose);
		XMStoreFloat4x4(&skull_reflection_cb.wvp, XMMatrixTranspose(wvp));
		XMStoreFloat4x4(&skull_reflection_cb.tex_transform, I);
		skull_reflection_cb.material = m_skull_mat;
		skull_reflection_cb.use_texture = skull_use_texture;
		skull_reflection_cb.alpha_clip = skull_alpha_clip;
		skull_reflection_cb.fog_enabled = skull_fog_enabled;
		m_object_cb.update(joj::Engine::s_renderer->get_device_context(), skull_reflection_cb);

		// Cache the old light directions, and reflect the light directions.
		joj::JFloat3 old_light_directions[3];
		for (i32 i = 0; i < 3; ++i)
		{
			old_light_directions[i] = m_dir_lights[i].direction;

			joj::JVector4 light_dir = XMLoadFloat3(&m_dir_lights[i].direction);
			joj::JVector4 reflected_light_dir = XMVector3TransformNormal(light_dir, R);
			XMStoreFloat3(&m_dir_lights[i].direction, reflected_light_dir);
		}

		// ---------------------------------------------------
		// Set Per Frame Constants
		// ---------------------------------------------------
		{
			joj::CBPerFrame frame_cb;
			frame_cb.dir_lights[0] = m_dir_lights[0];
			frame_cb.dir_lights[1] = m_dir_lights[1];
			frame_cb.dir_lights[2] = m_dir_lights[2];
			frame_cb.eye_posw = camera.m_position;
			frame_cb.fog_start = 2.0f;
			frame_cb.fog_range = 40.0f;
			frame_cb.fog_color = fog_color;
			frame_cb.light_count = m_light_count;
			m_frame_cb.update(joj::Engine::s_renderer->get_device_context(), frame_cb);
		}

		// Cull clockwise triangles for reflection.
		joj::Engine::s_renderer->get_device_context()->RSSetState(m_cull_clock_wise_RS);

		// Only draw reflection into visible mirror pixels as marked by the stencil buffer. 
		joj::Engine::s_renderer->get_device_context()->OMSetDepthStencilState(m_draw_reflection_DSS, 1);
		joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_skull_index_count, 0, 0);

		// Restore states.
		joj::Engine::s_renderer->get_device_context()->RSSetState(nullptr);
		joj::Engine::s_renderer->get_device_context()->OMSetDepthStencilState(nullptr, 0);

		// Restore light directions.
		for (i32 i = 0; i < 3; ++i)
		{
			m_dir_lights[i].direction = old_light_directions[i];
		}

		// ---------------------------------------------------
		// Set Per Frame Constants
		// ---------------------------------------------------
		{
			joj::CBPerFrame frame_cb;
			frame_cb.dir_lights[0] = m_dir_lights[0];
			frame_cb.dir_lights[1] = m_dir_lights[1];
			frame_cb.dir_lights[2] = m_dir_lights[2];
			frame_cb.eye_posw = camera.m_position;
			frame_cb.fog_start = 2.0f;
			frame_cb.fog_range = 40.0f;
			frame_cb.fog_color = fog_color;
			frame_cb.light_count = m_light_count;
			m_frame_cb.update(joj::Engine::s_renderer->get_device_context(), frame_cb);
		}
	}

	// ---------------------------------------------------
	// Draw Mirror to the back buffer as usual, but with transparency
	// ---------------------------------------------------
	{
		joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_room_vb.get_buffer(), &stride, &offset);

		joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_room_world);
		joj::JVector4 world_determinant = XMMatrixDeterminant(world);
		joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
		joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

		joj::JMatrix4x4 wvp = world * view * proj;

		joj::JMatrix4x4 I = joj::matrix4x4_identity();

		joj::CBPerObject mirror_cb;
		XMStoreFloat4x4(&mirror_cb.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&mirror_cb.world_inv_transpose, world_inv_transpose);
		XMStoreFloat4x4(&mirror_cb.wvp, XMMatrixTranspose(wvp));
		XMStoreFloat4x4(&mirror_cb.tex_transform, I);
		mirror_cb.material = m_mirror_mat;
		mirror_cb.use_texture = room_use_texture;
		mirror_cb.alpha_clip = room_alpha_clip;
		mirror_cb.fog_enabled = room_fog_enabled;
		m_object_cb.update(joj::Engine::s_renderer->get_device_context(), mirror_cb);

		joj::Engine::s_renderer->get_device_context()->PSSetShaderResources(0, 1, &m_mirror_map_SRV);

		// Draw Mirror
		joj::Engine::s_renderer->get_device_context()->OMSetBlendState(m_transparent_BS, blend_factor, 0xffffffff);
		joj::Engine::s_renderer->get_device_context()->Draw(6, 24);

		// Restore states.
		joj::Engine::s_renderer->get_device_context()->OMSetBlendState(nullptr, blend_factor, 0xffffffff);
		joj::Engine::s_renderer->get_device_context()->RSSetState(nullptr);
		joj::Engine::s_renderer->get_device_context()->OMSetDepthStencilState(nullptr, 0);
	}

	// ---------------------------------------------------
	// Draw Skull Shadow
	// ---------------------------------------------------
	{
		joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_skull_vb.get_buffer(), &stride, &offset);
		joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_skull_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

		using namespace DirectX;
		joj::JVector4 shadow_plane = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz plane
		joj::JVector4 to_main_light = -XMLoadFloat3(&m_dir_lights[0].direction);
		joj::JMatrix4x4 S = DirectX::XMMatrixShadow(shadow_plane, to_main_light);
		joj::JMatrix4x4 shadow_offsetY = DirectX::XMMatrixTranslation(0.0f, 0.001f, 0.0f);

		joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_room_world) * S * shadow_offsetY;
		joj::JVector4 world_determinant = XMMatrixDeterminant(world);
		joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
		joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

		joj::JMatrix4x4 wvp = world * view * proj;

		joj::JMatrix4x4 I = joj::matrix4x4_identity();

		joj::CBPerObject shadow_cb;
		XMStoreFloat4x4(&shadow_cb.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&shadow_cb.world_inv_transpose, world_inv_transpose);
		XMStoreFloat4x4(&shadow_cb.wvp, XMMatrixTranspose(wvp));
		XMStoreFloat4x4(&shadow_cb.tex_transform, I);
		shadow_cb.material = m_shadow_mat;
		shadow_cb.use_texture = skull_use_texture;
		shadow_cb.alpha_clip = skull_alpha_clip;
		shadow_cb.fog_enabled = skull_fog_enabled;
		m_object_cb.update(joj::Engine::s_renderer->get_device_context(), shadow_cb);

		// Draw Shadow
		joj::Engine::s_renderer->get_device_context()->OMSetDepthStencilState(m_no_double_blend_DSS, 0);
		joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_skull_index_count, 0, 0);

		// Restore default states.
		joj::Engine::s_renderer->get_device_context()->OMSetBlendState(nullptr, blend_factor, 0xffffffff);
		joj::Engine::s_renderer->get_device_context()->OMSetDepthStencilState(nullptr, 0);
	}

	joj::Engine::s_renderer->swap_buffers();
}

// ---------------------------------------------------------------------------------

void MirrorDemo::shutdown()
{
	m_floor_diffuse_map_SRV->Release();
	m_wall_diffuse_map_SRV->Release();
	m_mirror_map_SRV->Release();
	m_sampler_state->Release();
	m_wireframe_RS->Release();
	m_no_cull_RS->Release();
	m_cull_clock_wise_RS->Release();
	m_alpha_to_coverage_BS->Release();
	m_transparent_BS->Release();
	m_no_render_target_write_BS->Release();
	m_mark_mirror_DSS->Release();
	m_draw_reflection_DSS->Release();
	m_no_double_blend_DSS->Release();
	m_input_layout->Release();

	JINFO("Shutting down App...");
}

// ---------------------------------------------------------------------------------

#endif // JPLATFORM_WINDOWS