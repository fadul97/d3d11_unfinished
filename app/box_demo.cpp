#include "box_demo.h"

#if JPLATFORM_WINDOWS

#include "logger.h"
#include <DirectXColors.h>
#include <d3d11.h>
#include <string>
#include <d3dcompiler.h>

ID3DBlob* CompileShader(const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target)
{
	// use debug flags in debug mode
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ID3DBlob* byteCode = nullptr;
	ID3DBlob* errors;
	if (D3DCompileFromFile(filename.c_str(),
		defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(),
		target.c_str(),
		compileFlags,
		0,
		&byteCode,
		&errors) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to Compile Shader.");
	}

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
		JERROR(joj::ErrorCode::FAILED, "%s", (char*)errors->GetBufferPointer());
	}

	return byteCode;
}

void BoxDemo::init()
{
	constexpr f32 fov_angle = 45;
	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov_angle), 800.0f / 600.0f, 0.1f, 100.0f);
	DirectX::XMStoreFloat4x4(&mProj, P);
}

void BoxDemo::build_geometry_buffers(joj::D3D11Renderer& renderer)
{
	// Create vertex buffer
	Vertex vertices[] =
	{
		{DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT4(DirectX::Colors::White)},
		{DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f), DirectX::XMFLOAT4(DirectX::Colors::Black)},
		{DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f), DirectX::XMFLOAT4(DirectX::Colors::Red)},
		{DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f), DirectX::XMFLOAT4(DirectX::Colors::Green)},
		{DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f), DirectX::XMFLOAT4(DirectX::Colors::Blue)},
		{DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f), DirectX::XMFLOAT4(DirectX::Colors::Yellow)},
		{DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f), DirectX::XMFLOAT4(DirectX::Colors::Cyan)},
		{DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f), DirectX::XMFLOAT4(DirectX::Colors::Magenta)}
	};

	m_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(Vertex) * 8, vertices);

	if (renderer.get_device()->CreateBuffer(m_vb.get_buffer_desc(), m_vb.get_subdata(), &m_vb.get_buffer()) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Vertex Buffer.");
	}

	// Create the index buffer

	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	if (renderer.get_device()->CreateBuffer(&ibd, &iinitData, &m_box_ib) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Index Buffer.");
	}
}

void BoxDemo::build_shaders(joj::D3D11Renderer& renderer)
{
	// FIXME: Path is wrong
	m_shader.compile_vertex_shader(L"../../../../app/shaders/color.hlsl", "VS", "vs_5_0");
	m_shader.compile_pixel_shader(L"../../../../app/shaders/color.hlsl", "PS", "ps_5_0");

	renderer.get_device()->CreateVertexShader(
		// A pointer to the compiled shader
		m_shader.get_vsblob()->GetBufferPointer(),
		// Size of the compiled vertex shader
		m_shader.get_vsblob()->GetBufferSize(),
		// A pointer to a class linkage interface
		nullptr,
		// Address of a pointer to a ID3D11VertexShader interface
		&m_shader.get_vertex_shader());

	renderer.get_device()->CreatePixelShader(
		// A pointer to the compiled shader
		m_shader.get_psblob()->GetBufferPointer(),
		// Size of the compiled vertex shader
		m_shader.get_psblob()->GetBufferSize(),
		// A pointer to a class linkage interface
		nullptr,
		// Address of a pointer to a ID3D11VertexShader interface
		&m_shader.get_pixel_shader());
}

void BoxDemo::build_vertex_layout(joj::D3D11Renderer& renderer)
{
	// Create the vertex input layout.
	m_input_desc =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if (renderer.get_device()->CreateInputLayout(
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

void BoxDemo::build_constant_buffer(joj::D3D11Renderer& renderer)
{
	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(ObjectConstants);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Create the buffer.
	if (renderer.get_device()->CreateBuffer(&cbDesc, nullptr, &m_obj_cb) != S_OK)
	{
		JERROR(joj::ErrorCode::FAILED, "Failed to create Constant Buffer.");
	}
}

void BoxDemo::update(const f32 dt)
{
	float x = 2.0f;
	float y = 4.0f;
	float z = -05.0f;

	// Build the view matrix.
	DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);
}

void BoxDemo::draw()
{

}

void BoxDemo::shutdown()
{
	m_obj_cb->Release();
	m_input_layout->Release();

	m_box_ib->Release();
    
	JINFO("Shutting down App...");
}

#endif // JPLATFORM_WINDOWS