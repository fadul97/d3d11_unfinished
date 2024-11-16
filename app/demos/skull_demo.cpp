#include "skull_demo.h"

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

// ---------------------------------------------------------------------------------

f32 get_height(f32 x, f32 z)
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

// ---------------------------------------------------------------------------------

void SkullDemo::init()
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

	auto I = joj::matrix4x4_identity();
	XMStoreFloat4x4(&m_skull_world, I);

	build_geometry_buffers();
	build_shaders();
	build_vertex_layout();
	build_constant_buffer();
}

// ---------------------------------------------------------------------------------

void SkullDemo::build_geometry_buffers()
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

	f32 nx, ny, nz;
	joj::JFloat4 black(0.0f, 0.0f, 0.0f, 1.0f);

	// ---------------------------------------------------
	// Setup vertices vector for model file
	// ---------------------------------------------------

	std::vector<Vertex> vertices(vcount);
	for (u32 i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;

		vertices[i].Color = black;

		// Normal not used in this demo.
		fin >> nx >> ny >> nz;
	}

	// Create vertex buffer
	m_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(Vertex) * vcount, vertices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_vb.get_buffer_desc(), m_vb.get_subdata(), &m_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

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

	// Create the index buffer
	m_ib.setup(sizeof(u32) * m_skull_index_count, indices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_ib.get_buffer_desc(), m_ib.get_subdata(), &m_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}

	fin.close();

	// ---------------------------------------------------
	// Setup and add Skull Renderable Object
	// ---------------------------------------------------

	auto skull_ro = std::make_unique<joj::D3D11RenderableObject>();
	skull_ro->set_world_float4x4(m_skull_world);
	skull_ro->set_index_count(m_skull_index_count);
	skull_ro->set_index_location(0);
	skull_ro->set_vertex_location(0);
	m_scene.add_renderable_object(std::move(skull_ro));
}

// ---------------------------------------------------------------------------------

void SkullDemo::build_shaders()
{
	// FIXME: Path is wrong
	m_shader.compile_vertex_shader(L"../../../../app/shaders/color.hlsl", "VS", "vs_5_0");
	m_shader.compile_pixel_shader(L"../../../../app/shaders/color.hlsl", "PS", "ps_5_0");

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

void SkullDemo::build_vertex_layout()
{
	// Create the vertex input layout.
	m_input_desc =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

void SkullDemo::build_constant_buffer()
{
	m_cb.setup(joj::calculate_cb_byte_size(sizeof(joj::CBPerObject)), nullptr);

	// Create the buffer.
	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_cb.get_buffer_desc(), nullptr, &m_cb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Constant Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void SkullDemo::update(const f32 dt)
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

	DirectX::XMMATRIX V = camera.get_view_mat();
	XMStoreFloat4x4(&mView, V);
}

// ---------------------------------------------------------------------------------

void SkullDemo::draw()
{
	if (is_wireframe)
		joj::Engine::s_renderer->set_rasterizer_fill_mode(joj::RasterizerFillMode::Wireframe);
	else
		joj::Engine::s_renderer->set_rasterizer_fill_mode(joj::RasterizerFillMode::Solid);

	joj::Engine::s_renderer->clear();

	joj::Engine::s_renderer->get_device_context()->IASetInputLayout(m_input_layout);
	joj::Engine::s_renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
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

	joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(0, 1, &m_cb.get_buffer());

	// Set constants
	joj::JMatrix4x4 view = DirectX::XMLoadFloat4x4(&mView);
	joj::JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&mProj);

	m_scene.update(joj::Engine::s_frametime, &camera, mProj, m_cb);

	joj::Engine::s_renderer->swap_buffers();
}

// ---------------------------------------------------------------------------------

void SkullDemo::shutdown()
{
	m_input_layout->Release();

	JINFO("Shutting down App...");
}

// ---------------------------------------------------------------------------------

#endif // JPLATFORM_WINDOWS