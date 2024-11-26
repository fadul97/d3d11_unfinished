#include "crate_demo.h"

#if JPLATFORM_WINDOWS

// ---------------------------------------------------------------------------------

#include "logger.h"
#include "joj/engine.h"
#include "joj/resources/geometry/cube.h"
#include <renderer/d3d11/3d/DDSTextureLoader11.h>
#include "joj/renderer/d3d11/3d/cbuffer_structs_d3d11.h"

// ---------------------------------------------------------------------------------

void CrateDemo::init()
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
			DirectX::XMConvertToRadians(fov_angle), 800.0f / 600.0f, 0.1f, 100.0f
		);
		DirectX::XMStoreFloat4x4(&mProj, P);

		// Setup Mouse movement speed
		camera.m_movement_speed = 2.0f;
	}

	// ---------------------------------------------------
	// Initialize Matrixes
	// ---------------------------------------------------

	joj::JMatrix4x4 I = joj::matrix4x4_identity();
	XMStoreFloat4x4(&m_box_world, I);
	XMStoreFloat4x4(&m_tex_transform, I);

	// ---------------------------------------------------
	// Initialize Lights
	// ---------------------------------------------------

	m_dir_lights[0].ambient = joj::JFloat4(0.3f, 0.3f, 0.3f, 1.0f);
	m_dir_lights[0].diffuse = joj::JFloat4(0.8f, 0.8f, 0.8f, 1.0f);
	m_dir_lights[0].specular = joj::JFloat4(0.6f, 0.6f, 0.6f, 16.0f);
	m_dir_lights[0].direction = joj::JFloat3(0.707f, -0.707f, 0.0f);

	m_dir_lights[1].ambient = joj::JFloat4(0.2f, 0.2f, 0.2f, 1.0f);
	m_dir_lights[1].diffuse = joj::JFloat4(1.4f, 1.4f, 1.4f, 1.0f);
	m_dir_lights[1].specular = joj::JFloat4(0.3f, 0.3f, 0.3f, 16.0f);
	m_dir_lights[1].direction = joj::JFloat3(-0.707f, 0.0f, 0.707f);

	// ---------------------------------------------------
	// Initialize Materials
	// ---------------------------------------------------

	m_box_mat.ambient = joj::JFloat4(0.5f, 0.5f, 0.5f, 1.0f);
	m_box_mat.diffuse = joj::JFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	m_box_mat.specular = joj::JFloat4(0.6f, 0.6f, 0.6f, 16.0f);

	// ---------------------------------------------------
	// Build Resources
	// ---------------------------------------------------

	build_texture();
	build_geometry_buffers();
	build_shaders();
	build_vertex_layout();
	build_constant_buffers();
}

// ---------------------------------------------------------------------------------

void CrateDemo::build_texture()
{
	// ---------------------------------------------------
	// Create DDS Texture
	// ---------------------------------------------------

	if (DirectX::CreateDDSTextureFromFile(
		joj::Engine::s_renderer->get_device(),
		L"../../../../app/textures/WoodCrate01.dds",
		nullptr,
		&m_diffuse_map_SRV
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

	joj::Engine::s_renderer->get_device_context()->PSSetSamplers(0, 1, &m_sampler_state);
}

// ---------------------------------------------------------------------------------

void CrateDemo::build_geometry_buffers()
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
	m_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * total_vertex_count, vertices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_vb.get_buffer_desc(), m_vb.get_subdata(), &m_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// ---------------------------------------------------
	// Setup and Create Index Buffer
	// ---------------------------------------------------

	std::vector<u32> indices;
	indices.insert(indices.end(), std::begin(box.get_indices()), std::end(box.get_indices()));

	// Create the index buffer
	m_ib.setup(sizeof(u32) * total_index_count, indices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_ib.get_buffer_desc(), m_ib.get_subdata(), &m_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void CrateDemo::build_shaders()
{
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

void CrateDemo::build_vertex_layout()
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

void CrateDemo::build_constant_buffers()
{
	// ---------------------------------------------------
	// Setup and Create PerObject Constant Buffer
	// ---------------------------------------------------

	m_object_cb.setup(joj::calculate_cb_byte_size(sizeof(joj::CBPerObject)), nullptr);

	// Create the buffer.
	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_object_cb.get_buffer_desc(), nullptr, &m_object_cb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Constant Buffer.");
	}

	// ---------------------------------------------------
	// Setup and Create PerFrame Constant Buffer
	// ---------------------------------------------------

	m_frame_cb.setup(joj::calculate_cb_byte_size(sizeof(joj::CBPerFrame)), nullptr);

	// Create the buffer.
	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_frame_cb.get_buffer_desc(), nullptr, &m_frame_cb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Constant Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void CrateDemo::update(const f32 dt)
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
			int xoffset = cursorPos.x - centerX;
			int yoffset = centerY - cursorPos.y;

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

	if (joj::Engine::s_input->is_key_pressed('0'))
		m_light_count = 0;

	if (joj::Engine::s_input->is_key_pressed('1'))
		m_light_count = 1;

	if (joj::Engine::s_input->is_key_pressed('2'))
		m_light_count = 2;

	if (joj::Engine::s_input->is_key_pressed('T'))
		m_use_texture ^= 1;

	DirectX::XMMATRIX V = camera.get_view_mat();
	XMStoreFloat4x4(&mView, V);
}

// ---------------------------------------------------------------------------------

void CrateDemo::draw()
{
	if (is_wireframe)
		joj::Engine::s_renderer->set_rasterizer_fill_mode(joj::RasterizerFillMode::Wireframe);
	else
		joj::Engine::s_renderer->set_rasterizer_fill_mode(joj::RasterizerFillMode::Solid);

	joj::Engine::s_renderer->clear();

	joj::Engine::s_renderer->get_device_context()->IASetInputLayout(m_input_layout);
	joj::Engine::s_renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(joj::GeometryVertex);
	UINT offset = 0;
	joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_vb.get_buffer(), &stride, &offset);
	joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

	joj::Engine::s_renderer->get_device_context()->VSSetShader(
		// Pointer to a vertex shader
		m_shader.get_vertex_shader(),
		// A pointer to an array of class-instance interfaces
		nullptr,
		// The number of class-instance interfaces in the array
		0u);

	joj::Engine::s_renderer->get_device_context()->PSSetShader(
		// Pointer to a vertex shader
		m_shader.get_pixel_shader(),
		// A pointer to an array of class-instance interfaces
		nullptr,
		// The number of class-instance interfaces in the array
		0u);

	joj::Engine::s_renderer->get_device_context()->PSSetShaderResources(0, 1, &m_diffuse_map_SRV);
	joj::Engine::s_renderer->get_device_context()->PSSetSamplers(0, 1, &m_sampler_state);

	joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
	joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

	joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
	joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

	// ---------------------------------------------------
	// Set and Update PerFrame Constant
	// ---------------------------------------------------

	joj::CBPerFrame frame_cb;
	frame_cb.dir_lights[0] = m_dir_lights[0];
	frame_cb.dir_lights[1] = m_dir_lights[1];
	frame_cb.dir_lights[2] = m_dir_lights[1]; // Unecessary
	frame_cb.eye_posw = camera.m_position;
	frame_cb.fog_start = 0.0f;
	frame_cb.fog_color = joj::JFloat4{ 0.0f, 0.0f, 0.0f, 0.0f };
	frame_cb.fog_range = 0.0f;
	frame_cb.light_count = m_light_count;
	frame_cb.use_texture = m_use_texture;
	m_frame_cb.update(joj::Engine::s_renderer->get_device_context(), frame_cb);

	// ---------------------------------------------------
	// Set and Update PerObject Constant
	// ---------------------------------------------------

	joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_box_world);

	joj::JVector4 world_determinant = XMMatrixDeterminant(world);
	joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
	joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

	joj::JMatrix4x4 view = DirectX::XMLoadFloat4x4(&mView);
	joj::JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&mProj);
	joj::JMatrix4x4 wvp = world * view * proj;

	joj::CBPerObject cbPerObject;
	XMStoreFloat4x4(&cbPerObject.world, XMMatrixTranspose(world));
	XMStoreFloat4x4(&cbPerObject.world_inv_transpose, world_inv_transpose);
	XMStoreFloat4x4(&cbPerObject.wvp, XMMatrixTranspose(wvp));
	joj::JMatrix4x4 I = joj::matrix4x4_identity();
	XMStoreFloat4x4(&cbPerObject.tex_transform, I);
	cbPerObject.material = m_box_mat;
	m_object_cb.update(joj::Engine::s_renderer->get_device_context(), cbPerObject);

	joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_box_index_count, m_box_index_offset, m_box_vertex_offset);
	joj::Engine::s_renderer->swap_buffers();
}

// ---------------------------------------------------------------------------------

void CrateDemo::shutdown()
{
	// m_sampler_state->Release();
	m_diffuse_map_SRV->Release();
	m_input_layout->Release();

	JINFO("Shutting down App...");
}

// ---------------------------------------------------------------------------------

#endif // JPLATFORM_WINDOWS