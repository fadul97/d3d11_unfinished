#include "istancing_and_culling_demo.h"

#if JPLATFORM_WINDOWS

// ---------------------------------------------------------------------------------

#include "logger.h"
#include <DirectXColors.h>
#include <d3d11.h>
#include <string>
#include <d3dcompiler.h>
#include "joj/engine.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <renderer/d3d11/cbuffer_structs_d3d11.h>
#include <locale>
#include <codecvt>

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

struct Basic32
{
	joj::JFloat3 Pos;
	joj::JFloat3 Normal;
	joj::JFloat2 Tex;
};

const f32 Infinity = FLT_MAX;

// ---------------------------------------------------------------------------------

void IstancingAndCullingDemo::init()
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
		joj::JMatrix4x4 P = DirectX::XMMatrixPerspectiveFovLH(
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
	XMStoreFloat4x4(&mView, I);

	joj::JMatrix4x4 skull_scale = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	joj::JMatrix4x4 skull_offset = DirectX::XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	joj::JMatrix4x4 skull_world = XMMatrixMultiply(skull_scale, skull_offset);
	XMStoreFloat4x4(&m_skull_world, skull_world);

	constexpr f32 fov_angle = 45;
	joj::JMatrix4x4 P = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(fov_angle), 800.0f / 600.0f, 0.1f, 1000.0f
	);
	DirectX::BoundingFrustum::CreateFromMatrix(m_cam_frustum, P);

	// ---------------------------------------------------
	// Initialize Lights
	// ---------------------------------------------------

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

	m_skull_mat.ambient = joj::JFloat4(0.4f, 0.4f, 0.4f, 1.0f);
	m_skull_mat.diffuse = joj::JFloat4(0.8f, 0.8f, 0.8f, 1.0f);
	m_skull_mat.specular = joj::JFloat4(0.8f, 0.8f, 0.8f, 16.0f);

	// ---------------------------------------------------
	// Build Resources
	// ---------------------------------------------------

	build_shaders();
	build_vertex_layout();
	build_geometry_buffers();
	build_instanced_buffer();
	build_constant_buffers();
}

// ---------------------------------------------------------------------------------

void IstancingAndCullingDemo::build_geometry_buffers()
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

	joj::JFloat3 vMinf3(+Infinity, +Infinity, +Infinity);
	joj::JFloat3 vMaxf3(-Infinity, -Infinity, -Infinity);

	joj::JVector4 vMin = XMLoadFloat3(&vMinf3);
	joj::JVector4 vMax = XMLoadFloat3(&vMaxf3);

	// ---------------------------------------------------
	// Setup vertices vector for model file
	// ---------------------------------------------------

	std::vector<Basic32> vertices(vcount);
	for (u32 i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

		joj::JVector4 P = XMLoadFloat3(&vertices[i].Pos);

		vMin = DirectX::XMVectorMin(vMin, P);
		vMax = DirectX::XMVectorMax(vMax, P);
	}

	using namespace DirectX;
	XMStoreFloat3(&m_skull_box.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&m_skull_box.Extents, 0.5f * (vMax - vMin));

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	// ---------------------------------------------------
	// Setup indices vector for model file
	// ---------------------------------------------------

	m_skull_index_count = 3 * tcount;

	std::vector<u32> indices(m_skull_index_count);
	for (u32 i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	// ---------------------------------------------------
	// Setup and Create Vertex Buffer
	// ---------------------------------------------------

	// Create vertex buffer
	m_skull_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(Basic32) * vcount, vertices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(
		m_skull_vb.get_buffer_desc(),
		m_skull_vb.get_subdata(),
		&m_skull_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// ---------------------------------------------------
	// Setup and Create Index Buffer
	// ---------------------------------------------------

	// Create the index buffer
	m_skull_ib.setup(sizeof(u32) * m_skull_index_count, indices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(
		m_skull_ib.get_buffer_desc(),
		m_skull_ib.get_subdata(),
		&m_skull_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void IstancingAndCullingDemo::build_instanced_buffer()
{
	const i32 n = 5;
	m_instanced_data.resize(n * n * n);

	f32 width = 200.0f;
	f32 height = 200.0f;
	f32 depth = 200.0f;

	f32 x = -0.5f * width;
	f32 y = -0.5f * height;
	f32 z = -0.5f * depth;
	f32 dx = width / (n - 1);
	f32 dy = height / (n - 1);
	f32 dz = depth / (n - 1);
	for (i32 k = 0; k < n; ++k)
	{
		for (i32 i = 0; i < n; ++i)
		{
			for (i32 j = 0; j < n; ++j)
			{
				// Position instanced along a 3D grid.
				m_instanced_data[k * n * n + i * n + j].world = joj::JFloat4x4(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					x + j * dx, y + i * dy, z + k * dz, 1.0f);

				// Random color.
				m_instanced_data[k * n * n + i * n + j].color.x = RandF(0.0f, 1.0f);
				m_instanced_data[k * n * n + i * n + j].color.y = RandF(0.0f, 1.0f);
				m_instanced_data[k * n * n + i * n + j].color.z = RandF(0.0f, 1.0f);
				m_instanced_data[k * n * n + i * n + j].color.w = 1.0f;
			}
		}
	}

	m_instanced_buffer.setup(
		D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_WRITE,
		sizeof(InstancedData) * m_instanced_data.size(),
		nullptr
	);

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(
		m_instanced_buffer.get_buffer_desc(),
		nullptr,
		&m_instanced_buffer.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void IstancingAndCullingDemo::build_shaders()
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

void IstancingAndCullingDemo::build_vertex_layout()
{
	// Create the vertex input layout.
	m_input_desc =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 24, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "WORLD",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",    1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",    2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD",    3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
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

void IstancingAndCullingDemo::build_constant_buffers()
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

void IstancingAndCullingDemo::update(const f32 dt)
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
			speed = 20.0f;
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
	}

	joj::JMatrix4x4 V = camera.get_view_mat();
	XMStoreFloat4x4(&mView, V);

	if (joj::Engine::s_input->is_key_pressed('1'))
	{
		m_frustum_culling_enabled = !m_frustum_culling_enabled;
		JDEBUG("Frustum culling enabled = %d", m_frustum_culling_enabled);
	}

	m_visible_object_count = 0;

	if (m_frustum_culling_enabled)
	{
		joj::JVector4 detView = DirectX::XMMatrixDeterminant(V);
		joj::JMatrix4x4 invView = DirectX::XMMatrixInverse(&detView, V);

		D3D11_MAPPED_SUBRESOURCE mapped_data;
		if (joj::Engine::s_renderer->get_device_context()->Map(
			m_instanced_buffer.get_buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_data) != S_OK)
		{
			JERROR(joj::ErrorCode::FAILED, "Failed to map Subresource data.");
		}

		InstancedData* data_view = reinterpret_cast<InstancedData*>(mapped_data.pData);

		for (u32 i = 0; i < m_instanced_data.size(); ++i)
		{
			joj::JMatrix4x4 W = XMLoadFloat4x4(&m_instanced_data[i].world);
			auto wDet = XMMatrixDeterminant(W);
			joj::JMatrix4x4 invWorld = XMMatrixInverse(&wDet, W);

			// View space to the object's local space.
			joj::JMatrix4x4 toLocal = XMMatrixMultiply(invView, invWorld);

			// Decompose the matrix into its individual parts.
			joj::JVector4 scale;
			joj::JVector4 rotQuat;
			joj::JVector4 translation;
			if (!XMMatrixDecompose(&scale, &rotQuat, &translation, toLocal))
			{
				JERROR(joj::ErrorCode::FAILED, "Failed to decompose Matrix.");
			}

			// Transform the camera frustum from view space to the object's local space.
			DirectX::BoundingFrustum localspaceFrustum = m_cam_frustum;
			m_cam_frustum.Transform(localspaceFrustum, DirectX::XMVectorGetX(scale), rotQuat, translation);

			// Perform the box/frustum intersection test in local space.
			if (localspaceFrustum.Intersects(m_skull_box))
			{
				// Write the instance data to dynamic VB of the visible objects.
				data_view[m_visible_object_count++] = m_instanced_data[i];
			}
		}

		joj::Engine::s_renderer->get_device_context()->Unmap(m_instanced_buffer.get_buffer(), 0);
	}
	// No culling enabled, draw all objects
	else
	{
		D3D11_MAPPED_SUBRESOURCE mapped_data;
		if (joj::Engine::s_renderer->get_device_context()->Map(
			m_instanced_buffer.get_buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_data) != S_OK)
		{
			JERROR(joj::ErrorCode::FAILED, "Failed to map Subresource data.");
		}

		InstancedData* dataView = reinterpret_cast<InstancedData*>(mapped_data.pData);

		for (UINT i = 0; i < m_instanced_data.size(); ++i)
		{
			dataView[m_visible_object_count++] = m_instanced_data[i];
		}

		joj::Engine::s_renderer->get_device_context()->Unmap(m_instanced_buffer.get_buffer(), 0);
	}

	std::wostringstream outs;
	outs.precision(6);
	outs << L"Instancing and Culling Demo" <<
		L"    " << m_visible_object_count <<
		L" objects visible out of " << m_instanced_data.size();
	std::wstring wstr = outs.str();
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string inststr = converter.to_bytes(wstr);
	const char* cstr = inststr.c_str();
	JDEBUG("%s", cstr);
}

// ---------------------------------------------------------------------------------

void IstancingAndCullingDemo::draw()
{
	if (is_wireframe)
		joj::Engine::s_renderer->set_rasterizer_fill_mode(joj::RasterizerFillMode::Wireframe);
	else
		joj::Engine::s_renderer->set_rasterizer_fill_mode(joj::RasterizerFillMode::Solid);

	const joj::JFloat4 silver{ 0.75f, 0.75f, 0.75f, 1.0f };
	joj::Engine::s_renderer->clear(silver.x, silver.y, silver.z, silver.w);

	joj::Engine::s_renderer->get_device_context()->IASetInputLayout(m_input_layout);
	joj::Engine::s_renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
	joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

	joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
	joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

	joj::Engine::s_renderer->get_device_context()->PSSetSamplers(0, 1, &m_sampler_state);

	u32 stride[2] = { sizeof(Basic32), sizeof(InstancedData) };
	u32 offset[2] = { 0,0 };

	ID3D11Buffer* vbs[2] = { m_skull_vb.get_buffer(), m_instanced_buffer.get_buffer() };

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
		frame_cb.fog_start = 15.0f;
		frame_cb.fog_range = 175.0f;
		frame_cb.fog_color = silver;
		frame_cb.light_count = 3;
		joj::JMatrix4x4 vp = view * proj;
		XMStoreFloat4x4(&frame_cb.view_proj, XMMatrixTranspose(vp));
		m_frame_cb.update(joj::Engine::s_renderer->get_device_context(), frame_cb);
	}
	
	joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 2, vbs, stride, offset);
	joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_skull_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

	joj::Engine::s_renderer->get_device_context()->VSSetShader(m_shader.get_vertex_shader(), nullptr, 0u);
	joj::Engine::s_renderer->get_device_context()->PSSetShader(m_shader.get_pixel_shader(), nullptr, 0u);

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
	skull_cb.use_texture = 0;
	skull_cb.alpha_clip = 0;
	skull_cb.fog_enabled = 0;
	m_object_cb.update(joj::Engine::s_renderer->get_device_context(), skull_cb);

	joj::Engine::s_renderer->get_device_context()->DrawIndexedInstanced(
		m_skull_index_count, m_visible_object_count, 0, 0, 0);

	joj::Engine::s_renderer->swap_buffers();
}

// ---------------------------------------------------------------------------------

void IstancingAndCullingDemo::shutdown()
{
	m_input_layout->Release();

	JINFO("Shutting down App...");
}

// ---------------------------------------------------------------------------------

#endif // JPLATFORM_WINDOWS