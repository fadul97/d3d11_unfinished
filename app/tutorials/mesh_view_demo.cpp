#include "mesh_view_demo.h"
#include "joj/engine.h"
#include <renderer/d3d11/3d/cbuffer_structs_d3d11.h>

// ---------------------------------------------------------------------------------

void MeshViewDemo::init()
{
	// Basic Initialization
	{
		// Hide m_cam
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
		m_cam.m_movement_speed = 2.0f;
	}

	// ---------------------------------------------------
	// Initialize Lights
	// ---------------------------------------------------

	mDirLights[0].ambient = joj::JFloat4(0.6f, 0.6f, 0.6f, 1.0f);
	mDirLights[0].diffuse = joj::JFloat4(0.8f, 0.7f, 0.7f, 1.0f);
	mDirLights[0].specular = joj::JFloat4(0.6f, 0.6f, 0.7f, 1.0f);
	mDirLights[0].direction = joj::JFloat3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].ambient = joj::JFloat4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].diffuse = joj::JFloat4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirLights[1].specular = joj::JFloat4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].direction = joj::JFloat3(0.707f, -0.707f, 0.0f);

	mDirLights[2].ambient = joj::JFloat4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].diffuse = joj::JFloat4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[2].specular = joj::JFloat4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].direction = joj::JFloat3(0.0f, 0.0, -1.0f);

	mOriginalLightDir[0] = mDirLights[0].direction;
	mOriginalLightDir[1] = mDirLights[1].direction;
	mOriginalLightDir[2] = mDirLights[2].direction;

	// ---------------------------------------------------
	// Build Resources
	// ---------------------------------------------------

	build_shaders();
	build_vertex_layout();
	build_constant_buffers();
	build_model();
}

// ---------------------------------------------------------------------------------

void MeshViewDemo::build_shaders()
{
	// FIXME: Path is wrong
	m_shader.compile_vertex_shader(L"shaders/Demo.hlsl", "VS", "vs_5_0");
	m_shader.compile_pixel_shader(L"shaders/Demo.hlsl", "PS", "ps_5_0");

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

void MeshViewDemo::build_vertex_layout()
{
	m_input_desc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32,  D3D11_INPUT_PER_VERTEX_DATA, 0}
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

void MeshViewDemo::build_model()
{
	mTexMgr.init(joj::Engine::s_renderer->get_device());
	mBaseModel = new joj::D3D11BasicModel(
		joj::Engine::s_renderer->get_device(),
		mTexMgr,
		"../../../../app/models/base.m3d",
		L"../../../../app/textures/");
}

// ---------------------------------------------------------------------------------

void MeshViewDemo::build_constant_buffers()
{
	// ---------------------------------------------------
	// Setup PerObject Constant Buffer
	// ---------------------------------------------------

	m_object_cb.setup(joj::calculate_cb_byte_size(sizeof(joj::CBPerObject)), nullptr);

	// Create constant buffer
	if (joj::Engine::s_renderer->get_device()->CreateBuffer(
		m_object_cb.get_buffer_desc(),
		nullptr,
		&m_object_cb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Constant Buffer.");
	}

	// ---------------------------------------------------
	// Setup PerFrame Constant Buffer
	// ---------------------------------------------------

	m_frame_cb.setup(joj::calculate_cb_byte_size(sizeof(joj::CBPerFrame)), nullptr);

	// Create constant buffer
	if (joj::Engine::s_renderer->get_device()->CreateBuffer(
		m_frame_cb.get_buffer_desc(),
		nullptr,
		&m_frame_cb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Constant Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void MeshViewDemo::update(const f32 dt)
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
			m_cam.process_mouse_movement(xoffset, yoffset);
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
			m_cam.process_keyboard(joj::CameraMovement::FORWARD, dt * speed);

		if (joj::Engine::s_input->is_key_down(joj::KEY_S))
			m_cam.process_keyboard(joj::CameraMovement::BACKWARD, dt * speed);

		if (joj::Engine::s_input->is_key_down(joj::KEY_A))
			m_cam.process_keyboard(joj::CameraMovement::LEFT, dt * speed);

		if (joj::Engine::s_input->is_key_down(joj::KEY_D))
			m_cam.process_keyboard(joj::CameraMovement::RIGHT, dt * speed);

		DirectX::XMMATRIX V = m_cam.get_view_mat();
		XMStoreFloat4x4(&mView, V);
	}
}

// ---------------------------------------------------------------------------------

void MeshViewDemo::draw()
{
	joj::Engine::s_renderer->clear();

	u32 stride = sizeof(joj::Vertex::PosNormalTexTan);
	u32 offset = 0;

	joj::Engine::s_renderer->get_device_context()->IASetInputLayout(m_input_layout);
	joj::Engine::s_renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	joj::Engine::s_renderer->get_device_context()->VSSetShader(m_shader.get_vertex_shader(), nullptr, 0u);
	joj::Engine::s_renderer->get_device_context()->PSSetShader(m_shader.get_pixel_shader(), nullptr, 0u);

	joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
	joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

	joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
	joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

	// ---------------------------------------------------
	// Set Per Frame Constants
	// ---------------------------------------------------
	joj::CBPerFrame frame_cb;
	frame_cb.dir_lights[0] = mDirLights[0];
	frame_cb.dir_lights[1] = mDirLights[1];
	frame_cb.dir_lights[2] = mDirLights[2];
	frame_cb.eye_posw = m_cam.m_position;
	frame_cb.fog_start = 15.0f;
	frame_cb.fog_range = 175.0f;
	frame_cb.light_count = 3;
	m_frame_cb.update(joj::Engine::s_renderer->get_device_context(), frame_cb);

	// ---------------------------------------------------
	// Set Per Object Constants
	// ---------------------------------------------------
	joj::JMatrix4x4 I = joj::matrix4x4_identity();
	joj::JMatrix4x4 world = I;
	joj::JVector4 world_determinant = XMMatrixDeterminant(world);
	joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
	joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

	joj::JMatrix4x4 view = DirectX::XMLoadFloat4x4(&mView);
	joj::JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&mProj);
	joj::JMatrix4x4 wvp = world * view * proj;

	joj::CBPerObject model_cb;
	XMStoreFloat4x4(&model_cb.world, XMMatrixTranspose(world));
	XMStoreFloat4x4(&model_cb.world_inv_transpose, world_inv_transpose);
	XMStoreFloat4x4(&model_cb.wvp, XMMatrixTranspose(wvp));
	XMStoreFloat4x4(&model_cb.tex_transform, I);
	model_cb.material = mBaseModel->m_mat[0];
	model_cb.use_texture = 0;
	model_cb.alpha_clip = 0;
	model_cb.fog_enabled = 0;
	m_object_cb.update(joj::Engine::s_renderer->get_device_context(), model_cb);

	mBaseModel->m_model_mesh.draw(joj::Engine::s_renderer->get_device_context(), 0);

	joj::Engine::s_renderer->swap_buffers();
}

// ---------------------------------------------------------------------------------

void MeshViewDemo::shutdown()
{
	m_input_layout->Release();
}