#include "shapes_demo.h"

#if JPLATFORM_WINDOWS

#include "logger.h"
#include <DirectXColors.h>
#include <d3d11.h>
#include <string>
#include <d3dcompiler.h>
#include "joj/engine.h"

void ShapesDemo::init()
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

	auto I = joj::matrix4x4_identity();
	// auto I = DirectX::XMMatrixIdentity();
	joj::JMatrix4x4 box_scale = DirectX::XMMatrixScaling(2.0f, 1.0f, 2.0f);
	joj::JMatrix4x4 box_offset = DirectX::XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	joj::JMatrix4x4 box_world = XMMatrixMultiply(box_scale, box_offset);
	XMStoreFloat4x4(&m_box_world, box_world);

	XMStoreFloat4x4(&m_grid_world, I);

	for (i32 i = 0; i < 5; ++i)
	{
		DirectX::XMStoreFloat4x4(&m_cyl_world[i * 2 + 0], DirectX::XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
		DirectX::XMStoreFloat4x4(&m_cyl_world[i * 2 + 1], DirectX::XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));

		DirectX::XMStoreFloat4x4(&m_sphere_world[i * 2 + 0], DirectX::XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
		DirectX::XMStoreFloat4x4(&m_sphere_world[i * 2 + 1], DirectX::XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
	}

	build_geometry_buffers();
	build_shaders();
	build_vertex_layout();
	build_constant_buffer();
}

void ShapesDemo::build_geometry_buffers()
{
	joj::Cube box(1.0f, 1.0f, 1.0f);
	joj::Grid grid(20.0f, 30.0f, 60, 40);
	joj::Sphere sphere(0.5f, 20, 20);
	joj::Cylinder cylinder(0.5f, 0.3f, 3.0f, 20, 20);

	// ---------------------------------------------------
	// Setup Vertex Offsets
	// ---------------------------------------------------

	m_box_vertex_offset = 0;
	m_grid_vertex_offset = box.get_vertex_count();
	m_sphere_vertex_offset = m_grid_vertex_offset + grid.get_vertex_count();
	m_cylinder_vertex_offset = m_sphere_vertex_offset + sphere.get_vertex_count();
	
	// ---------------------------------------------------
	// Setup Index Count of each object
	// ---------------------------------------------------

	m_box_index_count = box.get_index_count();
	m_grid_index_count = grid.get_index_count();
	m_sphere_index_count = sphere.get_index_count();
	m_cylinder_index_count = cylinder.get_index_count();

	// ---------------------------------------------------
	// Setup Starting Index of each object
	// ---------------------------------------------------

	m_box_index_offset = 0;
	m_grid_index_offset = m_box_index_count;
	m_sphere_index_offset = m_grid_index_offset + m_grid_index_count;
	m_cylinder_index_offset = m_sphere_index_offset + m_sphere_index_count;

	const u32 total_vertex_count =
		box.get_vertex_count() +
		grid.get_vertex_count() +
		sphere.get_vertex_count() +
		cylinder.get_vertex_count();

	const u32 total_index_count =
		m_box_index_count +
		m_grid_index_count +
		m_sphere_index_count +
		m_cylinder_index_count;

	// ---------------------------------------------------
	// Setup and add Renderable Objects to vector
	// ---------------------------------------------------

	auto box_ro = std::make_unique<joj::D3D11RenderableObject>();
	box_ro->set_world_float4x4(m_box_world);
	box_ro->set_index_count(m_box_index_count);
	box_ro->set_index_location(m_box_index_offset);
	box_ro->set_vertex_location(m_box_vertex_offset);
	m_objects.push_back(std::move(box_ro));

	auto grid_ro = std::make_unique<joj::D3D11RenderableObject>();
	grid_ro->set_world_float4x4(m_grid_world);
	grid_ro->set_index_count(m_grid_index_count);
	grid_ro->set_index_location(m_grid_index_offset);
	grid_ro->set_vertex_location(m_grid_vertex_offset);
	m_objects.push_back(std::move(grid_ro));

	for (i32 i = 0; i < 5; ++i)
	{
		auto cyl_ro1 = std::make_unique<joj::D3D11RenderableObject>();
		cyl_ro1->set_world_float4x4(m_cyl_world[i * 2 + 0]);
		cyl_ro1->set_index_count(m_cylinder_index_count);
		cyl_ro1->set_index_location(m_cylinder_index_offset);
		cyl_ro1->set_vertex_location(m_cylinder_vertex_offset);
		m_objects.push_back(std::move(cyl_ro1));

		auto cyl_ro2 = std::make_unique<joj::D3D11RenderableObject>();
		cyl_ro2->set_world_float4x4(m_cyl_world[i * 2 + 1]);
		cyl_ro2->set_index_count(m_cylinder_index_count);
		cyl_ro2->set_index_location(m_cylinder_index_offset);
		cyl_ro2->set_vertex_location(m_cylinder_vertex_offset);
		m_objects.push_back(std::move(cyl_ro2));

		auto sphere_ro1 = std::make_unique<joj::D3D11RenderableObject>();
		sphere_ro1->set_world_float4x4(m_sphere_world[i * 2 + 0]);
		sphere_ro1->set_index_count(m_sphere_index_count);
		sphere_ro1->set_index_location(m_sphere_index_offset);
		sphere_ro1->set_vertex_location(m_sphere_vertex_offset);
		m_objects.push_back(std::move(sphere_ro1));

		auto sphere_ro2 = std::make_unique<joj::D3D11RenderableObject>();
		sphere_ro2->set_world_float4x4(m_sphere_world[i * 2 + 1]);
		sphere_ro2->set_index_count(m_sphere_index_count);
		sphere_ro2->set_index_location(m_sphere_index_offset);
		sphere_ro2->set_vertex_location(m_sphere_vertex_offset);
		m_objects.push_back(std::move(sphere_ro2));
	}

	// ---------------------------------------------------
	// Unique vector of GeometryVertex
	// ---------------------------------------------------

	std::vector<joj::GeometryVertex> vertices(total_vertex_count);

	// ---------------------------------------------------
	// Setup Position and Color for each object
	// ---------------------------------------------------

	const joj::JFloat4 black(0.0f, 0.0f, 0.0f, 1.0f);

	u32 k = 0;
	for (size_t i = 0; i < box.get_vertex_count(); ++i, ++k)
	{
		vertices[k].pos = box.get_vertex_data()[i].pos;
		vertices[k].color = black;
	}

	for (size_t i = 0; i < grid.get_vertex_count(); ++i, ++k)
	{
		vertices[k].pos = grid.get_vertex_data()[i].pos;
		vertices[k].color = black;
	}

	for (size_t i = 0; i < sphere.get_vertex_count(); ++i, ++k)
	{
		vertices[k].pos = sphere.get_vertex_data()[i].pos;
		vertices[k].color = black;
	}

	for (size_t i = 0; i < cylinder.get_vertex_count(); ++i, ++k)
	{
		vertices[k].pos = cylinder.get_vertex_data()[i].pos;
		vertices[k].color = black;
	}

	// Create vertex buffer
	m_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * total_vertex_count, vertices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_vb.get_buffer_desc(), m_vb.get_subdata(), &m_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// ---------------------------------------------------
	// Unique vector of indices
	// ---------------------------------------------------

	std::vector<u32> indices;
	indices.insert(indices.end(), std::begin(box.get_indices()), std::end(box.get_indices()));
	indices.insert(indices.end(), std::begin(grid.get_indices()), std::end(grid.get_indices()));
	indices.insert(indices.end(), std::begin(sphere.get_indices()), std::end(sphere.get_indices()));
	indices.insert(indices.end(), std::begin(cylinder.get_indices()), std::end(cylinder.get_indices()));


	// Create the index buffer
	m_ib.setup(sizeof(u32) * total_index_count, indices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_ib.get_buffer_desc(), m_ib.get_subdata(), &m_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

void ShapesDemo::build_shaders()
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

void ShapesDemo::build_vertex_layout()
{
	// Create the vertex input layout.
	m_input_desc =
	{
		// {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		// {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}

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

void ShapesDemo::build_constant_buffer()
{
	m_cb.setup(joj::calculate_cb_byte_size(sizeof(ObjectConstants)), nullptr);

	// Create the buffer.
	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_cb.get_buffer_desc(), nullptr, &m_cb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Constant Buffer.");
	}
}

void ShapesDemo::update(const f32 dt)
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
	// float x = 5.0f;
	// float y = 5.0f;
	// float z = -3.0f;

	// Build the view matrix.
	// DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
	// DirectX::XMVECTOR target = DirectX::XMVectorZero();
	// DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX V = camera.get_view_mat();
	XMStoreFloat4x4(&mView, V);
}

void ShapesDemo::draw()
{
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

	// Set constants
	joj::JMatrix4x4 view = DirectX::XMLoadFloat4x4(&mView);
	joj::JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&mProj);

	for (const auto& obj : m_objects)
	{
		joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&obj->get_world_float4x4());
		joj::JMatrix4x4 wvp = world * view * proj;
		
		ObjectConstants cbPerObject;
		XMStoreFloat4x4(&cbPerObject.World, XMMatrixTranspose(wvp));
		m_cb.update(joj::Engine::s_renderer->get_device_context(), cbPerObject);
		
		joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(0, 1, &m_cb.get_buffer());
	
		joj::Engine::s_renderer->get_device_context()->DrawIndexed(obj->get_index_count(), obj->get_index_location(), obj->get_vertex_location());
		
	}

	joj::Engine::s_renderer->swap_buffers();
}

void ShapesDemo::shutdown()
{
	m_input_layout->Release();

	JINFO("Shutting down App...");
}

#endif // JPLATFORM_WINDOWS