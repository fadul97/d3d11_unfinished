#include "box_demo.h"

#if JPLATFORM_WINDOWS

#include "logger.h"
#include <DirectXColors.h>
#include <d3d11.h>
#include <string>
#include <d3dcompiler.h>
#include "joj/engine.h"

void BoxDemo::init()
{
	constexpr f32 fov_angle = 45;
	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(fov_angle), 800.0f / 600.0f, 0.1f, 100.0f
	);
	DirectX::XMStoreFloat4x4(&mProj, P);

	build_geometry_buffers();
	build_shaders();
	build_vertex_layout();
	build_constant_buffer();

}

void BoxDemo::build_geometry_buffers()
{
	// Create vertex buffer
	m_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * geo.get_vertex_count(), geo.get_vertex_data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_vb.get_buffer_desc(), m_vb.get_subdata(), &m_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// Create the index buffer
	m_ib.setup(sizeof(u32) * geo.get_index_count(), geo.get_index_data());

	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_ib.get_buffer_desc(), m_ib.get_subdata(), &m_ib.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}

	geo_index_count = geo.get_index_count();
}

void BoxDemo::build_shaders()
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

void BoxDemo::build_vertex_layout()
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

void BoxDemo::build_constant_buffer()
{
	m_cb.setup(joj::calculate_cb_byte_size(sizeof(ObjectConstants)), nullptr);

	// Create the buffer.
	if (joj::Engine::s_renderer->get_device()->CreateBuffer(m_cb.get_buffer_desc(), nullptr, &m_cb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Constant Buffer.");
	}
}

void BoxDemo::update(const f32 dt)
{
	float x = 5.0f;
	float y = 5.0f;
	float z = -3.0f;

	// Build the view matrix.
	DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void BoxDemo::draw()
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
	joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&mWorld);
	joj::JMatrix4x4 view = DirectX::XMLoadFloat4x4(&mView);
	joj::JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&mProj);
	joj::JMatrix4x4 worldViewProj = world * view * proj;

	ObjectConstants cbPerObject;
	XMStoreFloat4x4(&cbPerObject.World, XMMatrixTranspose(worldViewProj));

	m_cb.update(joj::Engine::s_renderer->get_device_context(), cbPerObject);

	joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(0, 1, &m_cb.get_buffer());

	joj::Engine::s_renderer->get_device_context()->DrawIndexed(geo_index_count, 0, 0);
	joj::Engine::s_renderer->swap_buffers();
}

void BoxDemo::shutdown()
{
	m_input_layout->Release();

	JINFO("Shutting down App...");
}

#endif // JPLATFORM_WINDOWS