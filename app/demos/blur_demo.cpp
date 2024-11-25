#include "blur_demo.h"

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
#include <resources/geometry/quad.h>
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

struct TreePointSpriteVertex
{
	joj::JFloat3 Pos{ 0.0f, 0.0f, 0.0f };
	joj::JFloat2 Size{ 1.0f, 1.0f };
};

// ---------------------------------------------------------------------------------

void BlurDemo::init()
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
	XMStoreFloat4x4(&m_land_world, I);
	XMStoreFloat4x4(&m_waves_world, I);
	XMStoreFloat4x4(&mView, I);

	joj::JMatrix4x4 box_scale = DirectX::XMMatrixScaling(15.0f, 15.0f, 15.0f);
	joj::JMatrix4x4 box_offset = DirectX::XMMatrixTranslation(8.0f, 5.0f, -15.0f);
	joj::JMatrix4x4 combined = box_scale * box_offset;
	XMStoreFloat4x4(&m_box_world, combined);

	joj::JMatrix4x4 grass_tex_scale = DirectX::XMMatrixScaling(5.0f, 5.0f, 0.0f);
	XMStoreFloat4x4(&m_grass_tex_transform, grass_tex_scale);

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

	m_land_mat.ambient = joj::JFloat4(0.5f, 0.5f, 0.5f, 1.0f);
	m_land_mat.diffuse = joj::JFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	m_land_mat.specular = joj::JFloat4(0.2f, 0.2f, 0.2f, 16.0f);

	m_waves_mat.ambient = joj::JFloat4(0.5f, 0.5f, 0.5f, 1.0f);
	m_waves_mat.diffuse = joj::JFloat4(1.0f, 1.0f, 1.0f, 0.5f);
	m_waves_mat.specular = joj::JFloat4(0.8f, 0.8f, 0.8f, 32.0f);

	m_box_mat.ambient = joj::JFloat4(0.5f, 0.5f, 0.5f, 1.0f);
	m_box_mat.diffuse = joj::JFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	m_box_mat.specular = joj::JFloat4(0.4f, 0.4f, 0.4f, 16.0f);

	// ---------------------------------------------------
	// Build Resources
	// ---------------------------------------------------

	build_shaders();
	build_vertex_layout();
	build_render_states();
	build_textures();
	build_land_geometry_buffers();
	build_waves_geometry_buffers();
	build_crate_geometry_buffers();
	build_screen_quad_geometry_buffers();
	build_offscreen_views();
	build_constant_buffers();
}

// ---------------------------------------------------------------------------------

void BlurDemo::build_render_states()
{
	// ---------------------------------------------------
	// Describe and Create Wireframe Rasterizer State
	// ---------------------------------------------------
	m_render_state.create_rasterizer_state(joj::RasterizerStateOption::Wireframe);

	// ---------------------------------------------------
	// Describe and Create NoCull Rasterizer State
	// ---------------------------------------------------
	m_render_state.create_rasterizer_state(joj::RasterizerStateOption::NoCull);

	// ---------------------------------------------------
	// Describe and Create AlphaToCoverage Blend State
	// ---------------------------------------------------
	m_render_state.create_blend_state(joj::BlendStateOption::AlphaToCoverage);

	// ---------------------------------------------------
	// Describe and Create Transparent Blend State
	// ---------------------------------------------------
	m_render_state.create_blend_state(joj::BlendStateOption::Transparent);
}

// ---------------------------------------------------------------------------------

void BlurDemo::build_textures()
{
	// ---------------------------------------------------
	// Create Grass DDS Texture
	// ---------------------------------------------------

	if (DirectX::CreateDDSTextureFromFile(
		joj::Engine::s_renderer->get_device(),
		L"../../../../app/textures/grass.dds",
		nullptr,
		&m_grass_map_SRV
	) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create DDS Texture from file 'grass.dds'.");
	}
	else
	{
		JINFO("Created DDS Texture from file!");
	}

	// ---------------------------------------------------
	// Create Water DDS Texture
	// ---------------------------------------------------

	if (DirectX::CreateDDSTextureFromFile(
		joj::Engine::s_renderer->get_device(),
		L"../../../../app/textures/water2.dds",
		nullptr,
		&m_waves_map_SRV
	) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create DDS Texture from file 'water2.dds'.");
	}
	else
	{
		JINFO("Created DDS Texture from file!");
	}

	// ---------------------------------------------------
	// Create Crate DDS Texture
	// ---------------------------------------------------

	if (DirectX::CreateDDSTextureFromFile(
		joj::Engine::s_renderer->get_device(),
		L"../../../../app/textures/WireFence.dds",
		nullptr,
		&m_crate_map_SRV
	) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create DDS Texture from file 'WoodCrate01.dds'.");
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

void BlurDemo::build_land_geometry_buffers()
{
	joj::Grid grid(160.0f, 160.0f, 50, 50);

	// ---------------------------------------------------
	// Setup Grid Index Count
	// ---------------------------------------------------

	m_land_index_count = grid.get_index_count();

	// ---------------------------------------------------
	// GeometryVertex Vector of Grid vertices
	// ---------------------------------------------------

	std::vector<joj::GeometryVertex> vertices(grid.get_vertex_count());

	const joj::JFloat4 sand_light(1.0f, 0.96f, 0.62f, 1.0f);
	const joj::JFloat4 meadow_green(0.48f, 0.77f, 0.46f, 1.0f);
	const joj::JFloat4 forest_green(0.1f, 0.48f, 0.19f, 1.0f);
	const joj::JFloat4 earthy_brown(0.45f, 0.39f, 0.34f, 1.0f);
	const joj::JFloat4 snow(1.0f, 1.0f, 1.0f, 1.0);

	for (size_t i = 0; i < grid.get_vertex_count(); ++i)
	{
		joj::JFloat3 p = grid.get_vertex_data()[i].pos;

		p.y = get_hill_height(p.x, p.z);

		vertices[i].pos = p;
		vertices[i].normal = get_hill_normal(p.x, p.z);
		vertices[i].tex = grid.get_vertex_data()[i].tex;

		// Color the vertex based on its height.
		if (p.y < -10.0f)
		{
			// Sandy beach color.
			vertices[i].color = sand_light;
		}
		else if (p.y < 5.0f)
		{
			// Light yellow-green.
			vertices[i].color = meadow_green;
		}
		else if (p.y < 12.0f)
		{
			// Dark yellow-green.
			vertices[i].color = forest_green;
		}
		else if (p.y < 20.0f)
		{
			// Dark brown.
			vertices[i].color = earthy_brown;
		}
		else
		{
			// White snow.
			vertices[i].color = snow;
		}
	}

	// ---------------------------------------------------
	// Create Grid Vertex Buffer
	// ---------------------------------------------------

	m_land_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * grid.get_vertex_count(), vertices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_land_vb.get_buffer_desc(), m_land_vb.get_subdata(), &m_land_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// ---------------------------------------------------
	// Create Grid Index Buffer
	// ---------------------------------------------------

	m_land_ib.setup(sizeof(u32) * m_land_index_count, grid.get_index_data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_land_ib.get_buffer_desc(), m_land_ib.get_subdata(), &m_land_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void BlurDemo::build_waves_geometry_buffers()
{
	// ---------------------------------------------------
	// Create Waves Vertex Buffer
	// ---------------------------------------------------

	m_waves_vb.setup(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, sizeof(joj::GeometryVertex) * m_waves.get_vertex_count(), nullptr);

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_waves_vb.get_buffer_desc(), nullptr, &m_waves_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// ---------------------------------------------------
	// Create Waves Index Buffer
	// ---------------------------------------------------

	std::vector<u32> indices(3 * m_waves.get_triangle_count());

	// Iterate over each quad.
	u32 m = m_waves.get_row_count();
	u32 n = m_waves.get_column_count();
	i32 k = 0;
	for (u32 i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	m_waves_ib.setup(sizeof(u32) * indices.size(), indices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_waves_ib.get_buffer_desc(), m_waves_ib.get_subdata(), &m_waves_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void BlurDemo::build_crate_geometry_buffers()
{
	joj::Cube box(1.0f, 1.0f, 1.0f);

	// ---------------------------------------------------
	// Set Vertex Offset
	// ---------------------------------------------------

	m_box_vertex_offset = 0;

	// ---------------------------------------------------
	// Set Index Count
	// ---------------------------------------------------

	m_box_index_count = box.get_index_count();

	// ---------------------------------------------------
	// Set Index Offset
	// ---------------------------------------------------

	m_box_index_offset = 0;

	// ---------------------------------------------------
	// Setup Total Vertex and Index Count
	// ---------------------------------------------------

	const u32 total_vertex_count = box.get_vertex_count();

	const u32 total_index_count = m_box_index_count;

	// ---------------------------------------------------
	// Setup and Create Vertex Buffer
	// ---------------------------------------------------

	std::vector<joj::GeometryVertex> vertices(total_vertex_count);

	u32 k = 0;
	for (size_t i = 0; i < box.get_vertex_count(); ++i, ++k)
	{
		vertices[k].pos = box.get_vertex_data()[i].pos;
		vertices[k].normal = box.get_vertex_data()[i].normal;
		vertices[k].color = box.get_vertex_data()[i].color;
		vertices[k].tex = box.get_vertex_data()[i].tex;
	}

	// Create vertex buffer
	m_box_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * total_vertex_count, vertices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_box_vb.get_buffer_desc(), m_box_vb.get_subdata(), &m_box_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// ---------------------------------------------------
	// Setup and Create Index Buffer
	// ---------------------------------------------------

	std::vector<u32> indices;
	indices.insert(indices.end(), std::begin(box.get_indices()), std::end(box.get_indices()));

	// Create the index buffer
	m_box_ib.setup(sizeof(u32) * total_index_count, indices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_box_ib.get_buffer_desc(), m_box_ib.get_subdata(), &m_box_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void BlurDemo::build_screen_quad_geometry_buffers()
{
	joj::Quad quad;

	std::vector<joj::GeometryVertex> quad_vertices(4);
	std::vector<u32> quad_indices(6);

	// Position coordinates specified in NDC space.
	quad_vertices[0] = joj::GeometryVertex(
		-1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		-100.0f,
		0.0f, 1.0f);

	quad_vertices[1] = joj::GeometryVertex(
		-1.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		-100.0f,
		0.0f, 0.0f);

	quad_vertices[2] = joj::GeometryVertex(
		+1.0f, +1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		-100.0f,
		1.0f, 0.0f);

	quad_vertices[3] = joj::GeometryVertex(
		+1.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		-100.0f,
		1.0f, 1.0f);

	quad_indices[0] = 0;
	quad_indices[1] = 1;
	quad_indices[2] = 2;

	quad_indices[3] = 0;
	quad_indices[4] = 2;
	quad_indices[5] = 3;

	// ---------------------------------------------------
	// Setup and Create Vertex Buffer
	// ---------------------------------------------------

	std::vector<joj::GeometryVertex> vertices(4);
	for (UINT i = 0; i < quad_vertices.size(); ++i)
	{
		vertices[i].pos = quad_vertices[i].pos;
		vertices[i].normal = quad_vertices[i].normal;
		vertices[i].tex = quad_vertices[i].tex;
	}

	m_screen_quad_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * 4, vertices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(
		m_screen_quad_vb.get_buffer_desc(),
		m_screen_quad_vb.get_subdata(),
		&m_screen_quad_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// ---------------------------------------------------
	// Setup and Create Index Buffer
	// ---------------------------------------------------

	std::vector<u32> indices;
	indices.insert(indices.end(), std::begin(quad_indices), std::end(quad_indices));

	// Create the index buffer
	m_screen_quad_ib.setup(sizeof(u32) * 6, indices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(
		m_screen_quad_ib.get_buffer_desc(),
		m_screen_quad_ib.get_subdata(),
		&m_screen_quad_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void BlurDemo::build_offscreen_views()
{
	D3D11_TEXTURE2D_DESC tex_desc;

	tex_desc.Width = 800;
	tex_desc.Height = 600;
	tex_desc.MipLevels = 1;
	tex_desc.ArraySize = 1;
	tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.MiscFlags = 0;

	ID3D11Texture2D* offscreen_tex = 0;
	if (joj::Engine::s_renderer->get_device()->CreateTexture2D(&tex_desc, nullptr, &offscreen_tex) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Texture2D.");
	}

	if (joj::Engine::s_renderer->get_device()->CreateShaderResourceView(offscreen_tex, 0, &m_offscreen_SRV) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create ShaderResourceView.");
	}

	if (joj::Engine::s_renderer->get_device()->CreateRenderTargetView(offscreen_tex, 0, &m_offscreen_RTV) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create RenderTargetView.");
	}

	if (joj::Engine::s_renderer->get_device()->CreateUnorderedAccessView(offscreen_tex, 0, &m_offscreen_UAV) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create UnorderedAccessView.");
	}

	if (offscreen_tex)
		offscreen_tex->Release();
}

// ---------------------------------------------------------------------------------

void BlurDemo::build_shaders()
{
	// FIXME: Path is wrong
	m_vert_shader.compile_vertex_shader(L"../../../../app/shaders/Demo.hlsl", "VS", "vs_5_0");
	m_vert_shader.compile_pixel_shader(L"../../../../app/shaders/Demo.hlsl", "PS", "ps_5_0");
	m_vert_shader.compile_compute_shader(L"../../../../app/shaders/Blur.hlsl", "VertBlurCS", "cs_5_0");

	joj::Engine::s_renderer->get_device()->CreateVertexShader(
		// A pointer to the compiled shader
		m_vert_shader.get_vsblob()->GetBufferPointer(),
		// Size of the compiled vertex shader
		m_vert_shader.get_vsblob()->GetBufferSize(),
		// A pointer to a class linkage interface
		nullptr,
		// Address of a pointer to a ID3D11VertexShader interface
		&m_vert_shader.get_vertex_shader());

	joj::Engine::s_renderer->get_device()->CreatePixelShader(
		// A pointer to the compiled shader
		m_vert_shader.get_psblob()->GetBufferPointer(),
		// Size of the compiled vertex shader
		m_vert_shader.get_psblob()->GetBufferSize(),
		// A pointer to a class linkage interface
		nullptr,
		// Address of a pointer to a ID3D11VertexShader interface
		&m_vert_shader.get_pixel_shader());

	joj::Engine::s_renderer->get_device()->CreateComputeShader(
		// A pointer to the compiled shader
		m_vert_shader.get_csblob()->GetBufferPointer(),
		// Size of the compiled vertex shader
		m_vert_shader.get_csblob()->GetBufferSize(),
		// A pointer to a class linkage interface
		nullptr,
		// Address of a pointer to a ID3D11VertexShader interface
		&m_vert_shader.get_compute_shader());

	m_horz_shader.compile_vertex_shader(L"../../../../app/shaders/Demo.hlsl", "VS", "vs_5_0");
	m_horz_shader.compile_pixel_shader(L"../../../../app/shaders/Demo.hlsl", "PS", "ps_5_0");
	m_horz_shader.compile_compute_shader(L"../../../../app/shaders/Blur.hlsl", "HorzBlurCS", "cs_5_0");

	joj::Engine::s_renderer->get_device()->CreateVertexShader(
		// A pointer to the compiled shader
		m_horz_shader.get_vsblob()->GetBufferPointer(),
		// Size of the compiled vertex shader
		m_horz_shader.get_vsblob()->GetBufferSize(),
		// A pointer to a class linkage interface
		nullptr,
		// Address of a pointer to a ID3D11VertexShader interface
		&m_horz_shader.get_vertex_shader());

	joj::Engine::s_renderer->get_device()->CreatePixelShader(
		// A pointer to the compiled shader
		m_horz_shader.get_psblob()->GetBufferPointer(),
		// Size of the compiled vertex shader
		m_horz_shader.get_psblob()->GetBufferSize(),
		// A pointer to a class linkage interface
		nullptr,
		// Address of a pointer to a ID3D11VertexShader interface
		&m_horz_shader.get_pixel_shader());

	joj::Engine::s_renderer->get_device()->CreateComputeShader(
		// A pointer to the compiled shader
		m_horz_shader.get_csblob()->GetBufferPointer(),
		// Size of the compiled vertex shader
		m_horz_shader.get_csblob()->GetBufferSize(),
		// A pointer to a class linkage interface
		nullptr,
		// Address of a pointer to a ID3D11VertexShader interface
		&m_horz_shader.get_compute_shader());
}

// ---------------------------------------------------------------------------------

void BlurDemo::build_vertex_layout()
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
		m_vert_shader.get_vsblob()->GetBufferPointer(),
		m_vert_shader.get_vsblob()->GetBufferSize(),
		&m_input_layout
	) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Input Layout.");
	}
}

// ---------------------------------------------------------------------------------

void BlurDemo::build_constant_buffers()
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

void BlurDemo::update(const f32 dt)
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

	{
		// ---------------------------------------------------
		// Generate a random wave
		// ---------------------------------------------------

		if (joj::Engine::s_timer->total_elapsed() - prev >= 0.25f)
		{
			t_base += 0.25f;

			i32 i = 5 + rand() % (m_waves.get_row_count() - 10);
			i32 j = 5 + rand() % (m_waves.get_column_count() - 10);

			f32 r = RandF(0.5f, 1.0f);

			m_waves.disturb(i, j, r);
			prev = joj::Engine::s_timer->total_elapsed();
		}

		m_waves.update(dt);

		// ---------------------------------------------------
		// Update Waves Vertex Buffer with new solution
		// ---------------------------------------------------

		D3D11_MAPPED_SUBRESOURCE mappedData;
		if (joj::Engine::s_renderer->get_device_context()->Map(m_waves_vb.get_buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData) != S_OK)
		{
			JERROR(joj::ErrorCode::FAILED, "Failed to map Subresource data.");
		}

		joj::GeometryVertex* v = reinterpret_cast<joj::GeometryVertex*>(mappedData.pData);
		for (u32 i = 0; i < m_waves.get_vertex_count(); ++i)
		{
			v[i].pos = m_waves.Position(i);
			v[i].normal = m_waves.Normal(i);

			// Derive tex-coords in [0,1] from position.
			v[i].tex.x = 0.5f + m_waves.Position(i).x / m_waves.get_width();
			v[i].tex.y = 0.5f - m_waves.Position(i).z / m_waves.get_depth();
		}

		joj::Engine::s_renderer->get_device_context()->Unmap(m_waves_vb.get_buffer(), 0);

		// ---------------------------------------------------
		// Animate Water texture coordinates
		// ---------------------------------------------------

		// Tile water texture.
		joj::JMatrix4x4 waves_scale = DirectX::XMMatrixScaling(5.0f, 5.0f, 0.0f);

		// Translate texture over time.
		m_water_tex_offset.y += 0.05f * dt;
		m_water_tex_offset.x += 0.1f * dt;
		joj::JMatrix4x4 waves_offset = DirectX::XMMatrixTranslation(m_water_tex_offset.x, m_water_tex_offset.y, 0.0f);

		joj::JMatrix4x4 combined = waves_scale * waves_offset;

		// Combine scale and translation.
		DirectX::XMStoreFloat4x4(&m_water_tex_transform, combined);
	}

	// ---------------------------------------------------
	// Read Demo Input
	// ---------------------------------------------------
	{
		if (joj::Engine::s_input->is_key_pressed('1'))
		{
			m_render_options = joj::RenderOptions::Lighting;
			JDEBUG("Lighting");
		}

		if (joj::Engine::s_input->is_key_pressed('2'))
		{
			m_render_options = joj::RenderOptions::Textures;
			JDEBUG("Textures");
		}

		if (joj::Engine::s_input->is_key_pressed('3'))
		{
			m_render_options = joj::RenderOptions::TexturesAndFog;
			JDEBUG("TexturesAndFog");
		}

		// Increment Light count
		if (joj::Engine::s_input->is_key_pressed('L'))
			m_light_count = (m_light_count + 1) % 3;

		if (joj::Engine::s_input->is_key_pressed('R'))
			m_alpha_to_converage_on = !m_alpha_to_converage_on;
	}
}

// ---------------------------------------------------------------------------------

void BlurDemo::draw()
{
	/*
	ID3D11RenderTargetView* render_targets[1] = { m_offscreen_RTV };
	joj::Engine::s_renderer->get_device_context()->OMSetRenderTargets(
		1,
		render_targets,
		joj::Engine::s_renderer->get_depth_stencil_view()
	);
	*/

	// Color fog and clear color should be the same so it can actually look like a fog
	const joj::JFloat4 silver{ 0.75f, 0.75f, 0.75f, 1.0f };
	joj::Engine::s_renderer->clear(silver.x, silver.y, silver.z, silver.w);
	
	// ---------------------------------------------------
	// Draw the scene to the offscreen texture
	// ---------------------------------------------------
	draw_wapper();

	// ---------------------------------------------------
	// Restore Back Buffer. The offscreen render target will 
	// serve as an input into the compute shader for blurring,
	// so we must unbind it from the OM stage before we
	// can use it as an input into the compute shader.
	// ---------------------------------------------------
	/*
	render_targets[0] = joj::Engine::s_renderer->get_render_target_view();
	joj::Engine::s_renderer->get_device_context()->OMSetRenderTargets(
		1,
		render_targets,
		joj::Engine::s_renderer->get_depth_stencil_view()
	);
	*/

	// ---------------------------------------------------
	// Draw fullscreen quad with texture of blurred scene on it
	// ---------------------------------------------------
	// joj::Engine::s_renderer->clear(silver.x, silver.y, silver.z, silver.w);

	joj::Engine::s_renderer->swap_buffers();
}

// ---------------------------------------------------------------------------------

void BlurDemo::draw_wapper()
{
	joj::Engine::s_renderer->get_device_context()->IASetInputLayout(m_input_layout);
	joj::Engine::s_renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	float blend_factor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	u32 stride = sizeof(joj::GeometryVertex);
	u32 offset = 0;

	joj::JMatrix4x4 view = DirectX::XMLoadFloat4x4(&mView);
	joj::JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&mProj);
	joj::JMatrix4x4 vp = view * proj;

	// ---------------------------------------------------
	// Set Per Frame Constants
	// ---------------------------------------------------
	joj::CBPerFrame frame_cb;
	frame_cb.dir_lights[0] = m_dir_lights[0];
	frame_cb.dir_lights[1] = m_dir_lights[1];
	frame_cb.dir_lights[2] = m_dir_lights[2];
	frame_cb.eye_posw = camera.m_position;
	frame_cb.fog_start = 15.0f;
	frame_cb.fog_range = 175.0f;
	const joj::JFloat4 silver{ 0.75f, 0.75f, 0.75f, 1.0f };
	frame_cb.fog_color = silver;
	frame_cb.light_count = m_light_count;
	XMStoreFloat4x4(&frame_cb.view_proj, XMMatrixTranspose(vp));
	m_frame_cb.update(joj::Engine::s_renderer->get_device_context(), frame_cb);

	i32 box_use_texture = 0;
	i32 hills_use_texture = 0;

	i32 box_alpha_clip = 0;
	i32 hills_alpha_clip = 0;

	i32 box_fog_enabled = 0;
	i32 hills_fog_enabled = 0;

	switch (m_render_options)
	{
	case joj::RenderOptions::Lighting:
		box_use_texture = 0;
		box_alpha_clip = 0;
		box_fog_enabled = 0;

		hills_use_texture = 0;
		hills_alpha_clip = 0;
		hills_fog_enabled = 0;
		break;
	case joj::RenderOptions::Textures:
		box_use_texture = 1;
		box_alpha_clip = 1;
		box_fog_enabled = 0;

		hills_use_texture = 1;
		hills_alpha_clip = 0;
		hills_fog_enabled = 0;
		break;
	case joj::RenderOptions::TexturesAndFog:
		box_use_texture = 1;
		box_alpha_clip = 1;
		box_fog_enabled = 1;

		hills_use_texture = 1;
		hills_alpha_clip = 0;
		hills_fog_enabled = 1;
		break;
	default:
		break;
	}

	// ---------------------------------------------------
	// Draw Box with alpha clipping
	// ---------------------------------------------------
	{
		joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_box_vb.get_buffer(), &stride, &offset);
		joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_box_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

		joj::Engine::s_renderer->get_device_context()->VSSetShader(m_vert_shader.get_vertex_shader(), nullptr, 0u);
		joj::Engine::s_renderer->get_device_context()->PSSetShader(m_vert_shader.get_pixel_shader(), nullptr, 0u);
		joj::Engine::s_renderer->get_device_context()->CSSetShader(m_vert_shader.get_compute_shader(), nullptr, 0u);

		joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
		joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

		joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
		joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

		joj::Engine::s_renderer->get_device_context()->PSSetSamplers(0, 1, &m_sampler_state);

		joj::Engine::s_renderer->get_device_context()->PSSetShaderResources(0, 1, &m_crate_map_SRV);

		// -----------------------------------------------------------------------------
		joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_box_world);
		joj::JVector4 world_determinant = XMMatrixDeterminant(world);
		joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
		joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

		joj::JMatrix4x4 wvp = world * view * proj;

		joj::JMatrix4x4 I = joj::matrix4x4_identity();

		joj::CBPerObject box_cb;
		XMStoreFloat4x4(&box_cb.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&box_cb.world_inv_transpose, world_inv_transpose);
		XMStoreFloat4x4(&box_cb.wvp, XMMatrixTranspose(wvp));
		XMStoreFloat4x4(&box_cb.tex_transform, I);
		box_cb.material = m_box_mat;
		box_cb.use_texture = box_use_texture;
		box_cb.alpha_clip = box_alpha_clip;
		box_cb.fog_enabled = box_fog_enabled;
		m_object_cb.update(joj::Engine::s_renderer->get_device_context(), box_cb);
		// -----------------------------------------------------------------------------

		m_render_state.set_rasterizer_state(joj::RasterizerStateOption::NoCull);
		joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_box_index_count, 0, 0);
		m_render_state.set_rasterizer_state(joj::RasterizerStateOption::None);
	}

	// ---------------------------------------------------
	// Draw the hills and water with texture and fog (no alpha clipping needed).
	// ---------------------------------------------------
	{
		// ---------------------------------------------------
		// Draw Land
		// ---------------------------------------------------
		{
			joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_land_vb.get_buffer(), &stride, &offset);
			joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_land_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

			joj::Engine::s_renderer->get_device_context()->PSSetShaderResources(0, 1, &m_grass_map_SRV);

			// -----------------------------------------------------------------------------
			joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_land_world);
			joj::JVector4 world_determinant = XMMatrixDeterminant(world);
			joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
			joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

			joj::JMatrix4x4 wvp = world * view * proj;

			joj::CBPerObject land_cb;
			XMStoreFloat4x4(&land_cb.world, XMMatrixTranspose(world));
			XMStoreFloat4x4(&land_cb.world_inv_transpose, world_inv_transpose);
			XMStoreFloat4x4(&land_cb.wvp, XMMatrixTranspose(wvp));
			joj::JMatrix4x4 grass_textransf = XMLoadFloat4x4(&m_grass_tex_transform);
			joj::JMatrix4x4 grass_textrasnf_transposed = DirectX::XMMatrixTranspose(grass_textransf);
			XMStoreFloat4x4(&land_cb.tex_transform, grass_textrasnf_transposed);
			land_cb.material = m_land_mat;
			land_cb.use_texture = hills_use_texture;
			land_cb.alpha_clip = hills_alpha_clip;
			land_cb.fog_enabled = hills_fog_enabled;
			m_object_cb.update(joj::Engine::s_renderer->get_device_context(), land_cb);
			// -----------------------------------------------------------------------------

			joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_land_index_count, 0, 0);
		}

		// ---------------------------------------------------
		// Draw Waves
		// ---------------------------------------------------
		{
			joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_waves_vb.get_buffer(), &stride, &offset);
			joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_waves_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

			joj::Engine::s_renderer->get_device_context()->PSSetShaderResources(0, 1, &m_waves_map_SRV);

			joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_waves_world);
			joj::JVector4 world_determinant = XMMatrixDeterminant(world);
			joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
			joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

			joj::JMatrix4x4 wvp = world * view * proj;

			joj::CBPerObject waves_cb;
			XMStoreFloat4x4(&waves_cb.world, XMMatrixTranspose(world));
			XMStoreFloat4x4(&waves_cb.world_inv_transpose, world_inv);
			XMStoreFloat4x4(&waves_cb.wvp, XMMatrixTranspose(wvp));

			joj::JMatrix4x4 waves_textransf = XMLoadFloat4x4(&m_water_tex_transform);
			joj::JMatrix4x4 waves_textrasnf_transposed = DirectX::XMMatrixTranspose(waves_textransf);
			XMStoreFloat4x4(&waves_cb.tex_transform, waves_textrasnf_transposed);
			waves_cb.material = m_waves_mat;
			waves_cb.use_texture = hills_use_texture;
			waves_cb.alpha_clip = hills_alpha_clip;
			waves_cb.fog_enabled = hills_fog_enabled;
			m_object_cb.update(joj::Engine::s_renderer->get_device_context(), waves_cb);

			m_render_state.set_blend_state(joj::BlendStateOption::Transparent, blend_factor);
			joj::Engine::s_renderer->get_device_context()->DrawIndexed(3 * m_waves.get_triangle_count(), 0, 0);
			m_render_state.set_blend_state(joj::BlendStateOption::None, blend_factor);
		}
	}
}

// ---------------------------------------------------------------------------------

void BlurDemo::draw_screen_quad()
{
	joj::Engine::s_renderer->get_device_context()->IASetInputLayout(m_input_layout);
	joj::Engine::s_renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto diffuse_map = m_blur.get_blurred_output();
	joj::Engine::s_renderer->get_device_context()->PSSetShaderResources(0, 1, &diffuse_map);

	u32 stride = sizeof(joj::GeometryVertex);
	u32 offset = 0;

	joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_screen_quad_vb.get_buffer(), &stride, &offset);
	joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_screen_quad_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

	joj::Engine::s_renderer->get_device_context()->VSSetShader(m_horz_shader.get_vertex_shader(), nullptr, 0u);
	joj::Engine::s_renderer->get_device_context()->PSSetShader(m_horz_shader.get_pixel_shader(), nullptr, 0u);
	joj::Engine::s_renderer->get_device_context()->CSSetShader(m_horz_shader.get_compute_shader(), nullptr, 0u);

	joj::JMatrix4x4 I = joj::matrix4x4_identity();

	joj::JMatrix4x4 view = DirectX::XMLoadFloat4x4(&mView);
	joj::JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&mProj);
	joj::JMatrix4x4 vp = view * proj;

	joj::CBPerObject screenq;
	XMStoreFloat4x4(&screenq.world, XMMatrixTranspose(I));
	XMStoreFloat4x4(&screenq.world_inv_transpose, XMMatrixTranspose(I));
	XMStoreFloat4x4(&screenq.wvp, XMMatrixTranspose(I));
	XMStoreFloat4x4(&screenq.tex_transform, I);
	m_object_cb.update(joj::Engine::s_renderer->get_device_context(), screenq);

	joj::Engine::s_renderer->get_device_context()->DrawIndexed(6, 0, 0);
}

// ---------------------------------------------------------------------------------

void BlurDemo::shutdown()
{
	m_grass_map_SRV->Release();
	m_waves_map_SRV->Release();
	m_crate_map_SRV->Release();
	
	m_sampler_state->Release();
	m_input_layout->Release();

	JINFO("Shutting down App...");
}

// ---------------------------------------------------------------------------------

#endif // JPLATFORM_WINDOWS