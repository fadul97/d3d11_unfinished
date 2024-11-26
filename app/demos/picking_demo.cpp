#include "picking_demo.h"

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

const f32 Infinity = FLT_MAX;

// ---------------------------------------------------------------------------------

void PickingDemo::init()
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
			DirectX::XMConvertToRadians(fov_angle), 800.0f / 600.0f, 1.0f, 1000.0f
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

	joj::JMatrix4x4 mesh_scale = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	joj::JMatrix4x4 mesh_offset = DirectX::XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	joj::JMatrix4x4 mesh_world = XMMatrixMultiply(mesh_scale, mesh_offset);
	XMStoreFloat4x4(&m_mesh_world, mesh_world);

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

	m_mesh_mat.ambient = joj::JFloat4(0.4f, 0.4f, 0.4f, 1.0f);
	m_mesh_mat.diffuse = joj::JFloat4(0.8f, 0.8f, 0.8f, 1.0f);
	m_mesh_mat.specular = joj::JFloat4(0.8f, 0.8f, 0.8f, 16.0f);

	m_picked_triangle_mat.ambient = joj::JFloat4(0.0f, 0.8f, 0.4f, 1.0f);
	m_picked_triangle_mat.diffuse = joj::JFloat4(0.0f, 0.8f, 0.4f, 1.0f);
	m_picked_triangle_mat.specular = joj::JFloat4(0.0f, 0.0f, 0.0f, 16.0f);

	// ---------------------------------------------------
	// Build Resources
	// ---------------------------------------------------

	build_shaders();
	build_vertex_layout();
	build_render_states();
	build_geometry_buffers();
	build_constant_buffers();
}

// ---------------------------------------------------------------------------------

void PickingDemo::build_geometry_buffers()
{
	// ---------------------------------------------------
	// Read model file
	// ---------------------------------------------------

	std::ifstream fin("../../../../app/models/car.txt");
	if (!fin)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to find file 'car.txt'.");
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

	m_mesh_vertices.resize(vcount);

	// ---------------------------------------------------
	// Setup vertices vector for model file
	// ---------------------------------------------------

	for (u32 i = 0; i < vcount; ++i)
	{
		fin >> m_mesh_vertices[i].pos.x >> m_mesh_vertices[i].pos.y >> m_mesh_vertices[i].pos.z;
		fin >> m_mesh_vertices[i].normal.x >> m_mesh_vertices[i].normal.y >> m_mesh_vertices[i].normal.z;

		joj::JVector4 P = XMLoadFloat3(&m_mesh_vertices[i].pos);

		vMin = DirectX::XMVectorMin(vMin, P);
		vMax = DirectX::XMVectorMax(vMax, P);
	}

	using namespace DirectX;
	XMStoreFloat3(&m_mesh_box.Center, 0.5f * (vMin + vMax));
	XMStoreFloat3(&m_mesh_box.Extents, 0.5f * (vMax - vMin));

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	// ---------------------------------------------------
	// Setup indices vector for model file
	// ---------------------------------------------------

	m_mesh_index_count = 3 * tcount;

	m_mesh_indices.resize(m_mesh_index_count);
	for (u32 i = 0; i < tcount; ++i)
	{
		fin >> m_mesh_indices[i * 3 + 0] >> m_mesh_indices[i * 3 + 1] >> m_mesh_indices[i * 3 + 2];
	}

	fin.close();

	// ---------------------------------------------------
	// Setup and Create Vertex Buffer
	// ---------------------------------------------------

	// Create vertex buffer
	m_mesh_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(Basic32) * vcount, m_mesh_vertices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(
		m_mesh_vb.get_buffer_desc(),
		m_mesh_vb.get_subdata(),
		&m_mesh_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// ---------------------------------------------------
	// Setup and Create Index Buffer
	// ---------------------------------------------------

	// Create the index buffer
	m_mesh_ib.setup(sizeof(u32) * m_mesh_index_count, m_mesh_indices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(
		m_mesh_ib.get_buffer_desc(),
		m_mesh_ib.get_subdata(),
		&m_mesh_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void PickingDemo::build_shaders()
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

void PickingDemo::build_vertex_layout()
{
	// Create the vertex input layout.
	m_input_desc =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 24, D3D11_INPUT_PER_VERTEX_DATA,   0 },
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

void PickingDemo::build_render_states()
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

	// ---------------------------------------------------
	// Describe and Create Less Equal DepthStencil State
	// ---------------------------------------------------
	m_render_state.create_depthstencil_state(joj::DepthStencilStateOption::LessEqual);
}

// ---------------------------------------------------------------------------------

void PickingDemo::build_constant_buffers()
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

void PickingDemo::update(const f32 dt)
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

		/*
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
		*/

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

	if (joj::Engine::s_input->is_key_pressed('L'))
		m_light_count = (m_light_count + 1) % 3;

	if (joj::Engine::s_input->is_button_down(joj::BUTTON_RIGHT))
	{
		JDEBUG("Mouse pos = %d, %d", joj::Engine::s_input->get_xmouse(), joj::Engine::s_input->get_ymouse());
		pick(joj::Engine::s_input->get_xmouse(), joj::Engine::s_input->get_ymouse());
		JDEBUG("Picked = %d", m_picked_triangle);
	}
}

// ---------------------------------------------------------------------------------

void PickingDemo::draw()
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

	u32 stride = sizeof(Basic32);
	u32 offset = 0;

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
		frame_cb.light_count = m_light_count;
		m_frame_cb.update(joj::Engine::s_renderer->get_device_context(), frame_cb);
	}
	
	joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_mesh_vb.get_buffer(), &stride, &offset);
	joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_mesh_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

	joj::Engine::s_renderer->get_device_context()->VSSetShader(m_shader.get_vertex_shader(), nullptr, 0u);
	joj::Engine::s_renderer->get_device_context()->PSSetShader(m_shader.get_pixel_shader(), nullptr, 0u);

	joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_mesh_world);
	joj::JVector4 world_determinant = XMMatrixDeterminant(world);
	joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
	joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

	joj::JMatrix4x4 wvp = world * view * proj;

	joj::JMatrix4x4 I = joj::matrix4x4_identity();

	joj::CBPerObject mesh_cb;
	XMStoreFloat4x4(&mesh_cb.world, XMMatrixTranspose(world));
	XMStoreFloat4x4(&mesh_cb.world_inv_transpose, world_inv_transpose);
	XMStoreFloat4x4(&mesh_cb.wvp, XMMatrixTranspose(wvp));
	XMStoreFloat4x4(&mesh_cb.tex_transform, I);
	mesh_cb.material = m_mesh_mat;
	mesh_cb.use_texture = 0;
	mesh_cb.alpha_clip = 0;
	mesh_cb.fog_enabled = 0;
	m_object_cb.update(joj::Engine::s_renderer->get_device_context(), mesh_cb);

	joj::Engine::s_renderer->get_device_context()->DrawIndexed(
		m_mesh_index_count, 0, 0);

	// ---------------------------------------------------
	// Draw picked triangle
	// ---------------------------------------------------
	if (m_picked_triangle != -1)
	{
		m_render_state.set_depthstencil_state(joj::DepthStencilStateOption::LessEqual);
		mesh_cb.material = m_picked_triangle_mat;
		m_object_cb.update(joj::Engine::s_renderer->get_device_context(), mesh_cb);
		joj::Engine::s_renderer->get_device_context()->DrawIndexed(
			3, 3 * m_picked_triangle, 0);
		m_render_state.set_depthstencil_state(joj::DepthStencilStateOption::None);
	}

	joj::Engine::s_renderer->swap_buffers();
}

// ---------------------------------------------------------------------------------

void PickingDemo::pick(i32 sx, i32 sy)
{
	using namespace DirectX;

	constexpr f32 fov_angle = 45;
	joj::JMatrix4x4 P = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(fov_angle), 800.0f / 600.0f, 1.0f, 1000.0f
	);

	XMFLOAT4X4 Pfloat;
	XMStoreFloat4x4(&Pfloat, P);

	// Compute picking ray in view space.
	f32 vx = (+2.0f * sx / 800.0f - 1.0f) / Pfloat._11;
	f32 vy = (-2.0f * sy / 600.0f + 1.0f) / Pfloat._22;

	// Ray definition in view space.
	joj::JVector4 rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	joj::JVector4 rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// Tranform ray to local space of Mesh.
	joj::JMatrix4x4 V = camera.get_view_mat();
	auto vDet = XMMatrixDeterminant(V);
	joj::JMatrix4x4 invView = XMMatrixInverse(&vDet, V);

	joj::JMatrix4x4 W = XMLoadFloat4x4(&m_mesh_world);
	auto wDet = XMMatrixDeterminant(W);
	joj::JMatrix4x4 invWorld = XMMatrixInverse(&wDet, W);

	joj::JMatrix4x4 toLocal = XMMatrixMultiply(invView, invWorld);

	rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
	// so do the ray/triangle tests.
	//
	// If we did not hit the bounding box, then it is impossible that we hit 
	// the Mesh, so do not waste effort doing ray/triangle tests.

	// Assume we have not picked anything yet, so init to -1.
	m_picked_triangle = -1;
	f32 tmin = 0.0f;
	if (m_mesh_box.Intersects(rayOrigin, rayDir, tmin))
	{
		// Find the nearest ray/triangle intersection.
		tmin = Infinity;
		for (UINT i = 0; i < m_mesh_indices.size() / 3; ++i)
		{
			// Indices for this triangle.
			UINT i0 = m_mesh_indices[i * 3 + 0];
			UINT i1 = m_mesh_indices[i * 3 + 1];
			UINT i2 = m_mesh_indices[i * 3 + 2];

			// Vertices for this triangle.
			XMVECTOR v0 = XMLoadFloat3(&m_mesh_vertices[i0].pos);
			XMVECTOR v1 = XMLoadFloat3(&m_mesh_vertices[i1].pos);
			XMVECTOR v2 = XMLoadFloat3(&m_mesh_vertices[i2].pos);

			// We have to iterate over all the triangles in order to find the nearest intersection.
			float t = 0.0f;
			if (DirectX::TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, t))
			{
				if (t < tmin)
				{
					// This is the new nearest picked triangle.
					tmin = t;
					m_picked_triangle = i;
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------------

void PickingDemo::shutdown()
{
	m_input_layout->Release();

	JINFO("Shutting down App...");
}

// ---------------------------------------------------------------------------------

#endif // JPLATFORM_WINDOWS