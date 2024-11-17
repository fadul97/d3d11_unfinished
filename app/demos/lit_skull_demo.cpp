#include "lit_skull_demo.h"

#if JPLATFORM_WINDOWS

// ---------------------------------------------------------------------------------

#include "logger.h"
#include <string>
#include "joj/engine.h"
#include <fstream>
#include <string>
#include "joj/resources/geometry/cube.h"
#include <resources/geometry/sphere.h>
#include <resources/geometry/cylinder.h>
#include <resources/geometry/grid.h>
#include "joj/renderer/d3d11/renderable_object_d3d11.h"
#include "joj/renderer/d3d11/cbuffer_structs_d3d11.h"

// ---------------------------------------------------------------------------------

void LitSkullDemo::init()
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
	XMStoreFloat4x4(&m_grid_world, I);

	joj::JMatrix4x4 box_scale = DirectX::XMMatrixScaling(2.0f, 1.0f, 2.0f);
	joj::JMatrix4x4 box_offset = DirectX::XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	joj::JMatrix4x4 box_world = XMMatrixMultiply(box_scale, box_offset);
	XMStoreFloat4x4(&m_box_world, box_world);

	joj::JMatrix4x4 skull_scale = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	joj::JMatrix4x4 skull_offset = DirectX::XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	joj::JMatrix4x4 skull_world = XMMatrixMultiply(skull_scale, skull_offset);
	XMStoreFloat4x4(&m_skull_world, skull_world);

	for (i32 i = 0; i < 5; ++i)
	{
		DirectX::XMStoreFloat4x4(&m_cyl_world[i * 2 + 0], DirectX::XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
		DirectX::XMStoreFloat4x4(&m_cyl_world[i * 2 + 1], DirectX::XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));

		DirectX::XMStoreFloat4x4(&m_sphere_world[i * 2 + 0], DirectX::XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
		DirectX::XMStoreFloat4x4(&m_sphere_world[i * 2 + 1], DirectX::XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
	}

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

	m_grid_mat.ambient = joj::JFloat4(0.48f, 0.77f, 0.46f, 1.0f);
	m_grid_mat.diffuse = joj::JFloat4(0.48f, 0.77f, 0.46f, 1.0f);
	m_grid_mat.specular = joj::JFloat4(0.2f, 0.2f, 0.2f, 16.0f);

	m_cylinder_mat.ambient = joj::JFloat4(0.7f, 0.85f, 0.7f, 1.0f);
	m_cylinder_mat.diffuse = joj::JFloat4(0.7f, 0.85f, 0.7f, 1.0f);
	m_cylinder_mat.specular = joj::JFloat4(0.8f, 0.8f, 0.8f, 16.0f);

	m_sphere_mat.ambient = joj::JFloat4(0.1f, 0.2f, 0.3f, 1.0f);
	m_sphere_mat.diffuse = joj::JFloat4(0.2f, 0.4f, 0.6f, 1.0f);
	m_sphere_mat.specular = joj::JFloat4(0.9f, 0.9f, 0.9f, 16.0f);

	m_box_mat.ambient = joj::JFloat4(0.651f, 0.5f, 0.392f, 1.0f);
	m_box_mat.diffuse = joj::JFloat4(0.651f, 0.5f, 0.392f, 1.0f);
	m_box_mat.specular = joj::JFloat4(0.2f, 0.2f, 0.2f, 16.0f);

	m_skull_mat.ambient = joj::JFloat4(0.8f, 0.8f, 0.8f, 1.0f);
	m_skull_mat.diffuse = joj::JFloat4(0.8f, 0.8f, 0.8f, 1.0f);
	m_skull_mat.specular = joj::JFloat4(0.8f, 0.8f, 0.8f, 16.0f);

	// ---------------------------------------------------
	// Build Resources
	// ---------------------------------------------------

	build_shapes_geometry_buffers();
	build_skull_geometry_buffer();
	build_shaders();
	build_vertex_layout();
	build_constant_buffers();
}

// ---------------------------------------------------------------------------------

void LitSkullDemo::build_shapes_geometry_buffers()
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
	// Unique vector of GeometryVertex
	// ---------------------------------------------------

	std::vector<joj::GeometryVertex> vertices(total_vertex_count);

	// ---------------------------------------------------
	// Setup Position and Color for each object
	// ---------------------------------------------------

	const joj::JFloat4 black(0.0f, 0.0f, 0.0f, 1.0f);
	const joj::JFloat4 blue(0.0f, 0.0f, 1.0f, 1.0f);
	const joj::JFloat4 purple(1.0f, 0.0f, 1.0f, 1.0f);
	const joj::JFloat4 yellow(1.0f, 1.0f, 0.0f, 1.0f);

	u32 k = 0;
	for (size_t i = 0; i < box.get_vertex_count(); ++i, ++k)
	{
		vertices[k].pos = box.get_vertex_data()[i].pos;
		vertices[k].normal = box.get_vertex_data()[i].normal;
		vertices[k].color = purple; // Not using in Demo.hlsl shader file
	}

	for (size_t i = 0; i < grid.get_vertex_count(); ++i, ++k)
	{
		vertices[k].pos = grid.get_vertex_data()[i].pos;
		vertices[k].normal = grid.get_vertex_data()[i].normal;
		vertices[k].color = black; // Not using in Demo.hlsl shader file
	}

	for (size_t i = 0; i < sphere.get_vertex_count(); ++i, ++k)
	{
		vertices[k].pos = sphere.get_vertex_data()[i].pos;
		vertices[k].normal = sphere.get_vertex_data()[i].normal;
		vertices[k].color = yellow; // Not using in Demo.hlsl shader file
	}

	for (size_t i = 0; i < cylinder.get_vertex_count(); ++i, ++k)
	{
		vertices[k].pos = cylinder.get_vertex_data()[i].pos;
		vertices[k].normal = cylinder.get_vertex_data()[i].normal;
		vertices[k].color = blue; // Not using in Demo.hlsl shader file
	}

	// Create vertex buffer
	m_shapes_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * total_vertex_count, vertices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_shapes_vb.get_buffer_desc(), m_shapes_vb.get_subdata(), &m_shapes_vb.get_buffer()) != S_OK)
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
	m_shapes_ib.setup(sizeof(u32) * total_index_count, indices.data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_shapes_ib.get_buffer_desc(), m_shapes_ib.get_subdata(), &m_shapes_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

// ---------------------------------------------------------------------------------

void LitSkullDemo::build_skull_geometry_buffer()
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

void LitSkullDemo::build_shaders()
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

void LitSkullDemo::build_vertex_layout()
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

void LitSkullDemo::build_constant_buffers()
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

void LitSkullDemo::update(const f32 dt)
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

	if (joj::Engine::s_input->is_key_pressed('3'))
		m_light_count = 3;

	joj::JMatrix4x4 V = camera.get_view_mat();
	XMStoreFloat4x4(&mView, V);
}

// ---------------------------------------------------------------------------------

void LitSkullDemo::draw()
{
	if (is_wireframe)
		joj::Engine::s_renderer->set_rasterizer_fill_mode(joj::RasterizerFillMode::Wireframe);
	else
		joj::Engine::s_renderer->set_rasterizer_fill_mode(joj::RasterizerFillMode::Solid);

	joj::Engine::s_renderer->clear();

	joj::Engine::s_renderer->get_device_context()->IASetInputLayout(m_input_layout);
	joj::Engine::s_renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	u32 stride = sizeof(joj::GeometryVertex);
	u32 offset = 0;
	joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_shapes_vb.get_buffer(), &stride, &offset);
	joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_shapes_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

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
	
	joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
	joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

	joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(0, 1, &m_object_cb.get_buffer());
	joj::Engine::s_renderer->get_device_context()->PSSetConstantBuffers(1, 1, &m_frame_cb.get_buffer());

	// ---------------------------------------------------
	// Set and Update PerFrame Constants
	// ---------------------------------------------------

	joj::JMatrix4x4 view = DirectX::XMLoadFloat4x4(&mView);
	joj::JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&mProj);

	joj::CBPerFrame frame_cb;
	frame_cb.dir_lights[0] = m_dir_lights[0];
	frame_cb.dir_lights[1] = m_dir_lights[1];
	frame_cb.dir_lights[2] = m_dir_lights[2];
	frame_cb.eye_posw = camera.m_position;
	frame_cb.light_count = m_light_count;
	frame_cb.fog_start = 0.0f;
	frame_cb.fog_color = 0.0f;
	frame_cb.fog_range = 0.0f;
	m_frame_cb.update(joj::Engine::s_renderer->get_device_context(), frame_cb);

	// ---------------------------------------------------
	// Draw Grid
	// ---------------------------------------------------

	joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&m_grid_world);

	joj::JVector4 world_determinant = XMMatrixDeterminant(world);
	joj::JMatrix4x4 world_inv = XMMatrixInverse(&world_determinant, world);
	joj::JMatrix4x4 world_inv_transpose = XMMatrixTranspose(world_inv);

	joj::JMatrix4x4 wvp = world * view * proj;

	joj::CBPerObject cbPerObject;
	XMStoreFloat4x4(&cbPerObject.world, XMMatrixTranspose(world));
	XMStoreFloat4x4(&cbPerObject.world_inv_transpose, world_inv_transpose);
	XMStoreFloat4x4(&cbPerObject.wvp, XMMatrixTranspose(wvp));
	cbPerObject.material = m_grid_mat;
	m_object_cb.update(joj::Engine::s_renderer->get_device_context(), cbPerObject);

	joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_grid_index_count, m_grid_index_offset, m_grid_vertex_offset);

	// ---------------------------------------------------
	// Draw Box
	// ---------------------------------------------------

	world = DirectX::XMLoadFloat4x4(&m_box_world);

	world_determinant = XMMatrixDeterminant(world);
	world_inv = XMMatrixInverse(&world_determinant, world);
	world_inv_transpose = XMMatrixTranspose(world_inv);

	wvp = world * view * proj;
	
	XMStoreFloat4x4(&cbPerObject.world, XMMatrixTranspose(world));
	XMStoreFloat4x4(&cbPerObject.world_inv_transpose, world_inv_transpose);
	XMStoreFloat4x4(&cbPerObject.wvp, XMMatrixTranspose(wvp));
	cbPerObject.material = m_box_mat;
	m_object_cb.update(joj::Engine::s_renderer->get_device_context(), cbPerObject);

	joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_box_index_count, m_box_index_offset, m_box_vertex_offset);

	// ---------------------------------------------------
	// Draw Cylinders
	// ---------------------------------------------------

	for (i32 i = 0; i < 10; ++i)
	{
		world = XMLoadFloat4x4(&m_cyl_world[i]);
		world_determinant = XMMatrixDeterminant(world);
		world_inv = XMMatrixInverse(&world_determinant, world);
		world_inv_transpose = XMMatrixTranspose(world_inv);

		wvp = world * view * proj;

		XMStoreFloat4x4(&cbPerObject.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&cbPerObject.world_inv_transpose, world_inv_transpose);
		XMStoreFloat4x4(&cbPerObject.wvp, XMMatrixTranspose(wvp));
		cbPerObject.material = m_cylinder_mat;
		m_object_cb.update(joj::Engine::s_renderer->get_device_context(), cbPerObject);

		joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_cylinder_index_count, m_cylinder_index_offset, m_cylinder_vertex_offset);
	}

	// ---------------------------------------------------
	// Draw Spheres
	// ---------------------------------------------------

	for (i32 i = 0; i < 10; ++i)
	{
		world = XMLoadFloat4x4(&m_sphere_world[i]);
		world_determinant = XMMatrixDeterminant(world);
		world_inv = XMMatrixInverse(&world_determinant, world);
		world_inv_transpose = XMMatrixTranspose(world_inv);

		wvp = world * view * proj;

		XMStoreFloat4x4(&cbPerObject.world, XMMatrixTranspose(world));
		XMStoreFloat4x4(&cbPerObject.world_inv_transpose, world_inv_transpose);
		XMStoreFloat4x4(&cbPerObject.wvp, XMMatrixTranspose(wvp));
		cbPerObject.material = m_sphere_mat;
		m_object_cb.update(joj::Engine::s_renderer->get_device_context(), cbPerObject);

		joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_sphere_index_count, m_sphere_index_offset, m_sphere_vertex_offset);
	}

	// ---------------------------------------------------
	// Draw Skull
	// ---------------------------------------------------

	joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &m_skull_vb.get_buffer(), &stride, &offset);
	joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(m_skull_ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

	world = DirectX::XMLoadFloat4x4(&m_skull_world);

	world_determinant = XMMatrixDeterminant(world);
	world_inv = XMMatrixInverse(&world_determinant, world);
	world_inv_transpose = XMMatrixTranspose(world_inv);

	wvp = world * view * proj;

	XMStoreFloat4x4(&cbPerObject.world, XMMatrixTranspose(world));
	XMStoreFloat4x4(&cbPerObject.world_inv_transpose, world_inv_transpose);
	XMStoreFloat4x4(&cbPerObject.wvp, XMMatrixTranspose(wvp));
	cbPerObject.material = m_skull_mat;
	m_object_cb.update(joj::Engine::s_renderer->get_device_context(), cbPerObject);

	joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_skull_index_count, 0, 0);

	joj::Engine::s_renderer->swap_buffers();
}

// ---------------------------------------------------------------------------------

void LitSkullDemo::shutdown()
{
	m_input_layout->Release();

	JINFO("Shutting down App...");
}

// ---------------------------------------------------------------------------------

#endif // JPLATFORM_WINDOWS