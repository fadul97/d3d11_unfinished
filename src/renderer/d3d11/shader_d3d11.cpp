#include "renderer/d3d11/shader_d3d11.h"

#if JPLATFORM_WINDOWS

#include <d3dcompiler.h>
#include "logger.h"

joj::D3D11Shader::D3D11Shader()
{
	m_vertex_shader = nullptr;
	m_pixel_shader = nullptr;
	m_geometry_shader = nullptr;
	m_compute_shader = nullptr;
	m_vsblob = nullptr;
	m_psblob = nullptr;
	m_gsblob = nullptr;
	m_csblob = nullptr;
}

joj::D3D11Shader::~D3D11Shader()
{
	// Relase Direct3D resources
	if (m_psblob)
	{
		m_psblob->Release();
		m_psblob = nullptr;
	}

	if (m_gsblob)
	{
		m_gsblob->Release();
		m_gsblob = nullptr;
	}

	if (m_vsblob)
	{
		m_vsblob->Release();
		m_vsblob = nullptr;
	}

	// Relase Pixel Shader
	if (m_pixel_shader)
	{
		m_pixel_shader->Release();
		m_pixel_shader = nullptr;
	}

	// Relase Geometry Shader
	if (m_geometry_shader)
	{
		m_geometry_shader->Release();
		m_geometry_shader = nullptr;
	}

	// Relase Vertex Shader
	if (m_vertex_shader)
	{
		m_vertex_shader->Release();
		m_vertex_shader = nullptr;
	}
}

void joj::D3D11Shader::compile_vertex_shader(const WCHAR* vertex_path, LPCSTR entry_point, LPCSTR shader_model)
{
	DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Let compiler insert debug information into the output code
	shader_flags |= D3DCOMPILE_DEBUG;

	// Disable optimizations
	shader_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;    // Compiler will not validate the generated code -> Recommended to use only with successfully compiled shaders
#endif // !_DEBUG

	// --------------------------------
	// Vertex Shader
	// --------------------------------

	ID3DBlob* shader_compile_errors_blob = nullptr;          // To get info about compilation

	// Compile Vertex Shader
	if (D3DCompileFromFile(vertex_path, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, shader_model, shader_flags, NULL, &m_vsblob, &shader_compile_errors_blob) != S_OK)
	{
		if (shader_compile_errors_blob != nullptr)
		{
			OutputDebugStringA((char*)shader_compile_errors_blob->GetBufferPointer());
			JERROR(ErrorCode::ERR_SHADER_D3D11_VERTEX_COMPILATION, "%s", (char*)shader_compile_errors_blob->GetBufferPointer());
			shader_compile_errors_blob->Release();
		}
		else
		{
			JERROR(ErrorCode::ERR_SHADER_D3D11_VERTEX_COMPILATION, "Failed to compile Vertex Shader.");
		}
	}

	if (shader_compile_errors_blob != nullptr)
	{
		shader_compile_errors_blob->Release();
	}
}

void joj::D3D11Shader::compile_pixel_shader(const WCHAR* pixel_path, LPCSTR entry_point, LPCSTR shader_model)
{
	DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Let compiler insert debug information into the output code
	shader_flags |= D3DCOMPILE_DEBUG;

	// Disable optimizations
	shader_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;    // Compiler will not validate the generated code -> Recommended to use only with successfully compiled shaders
#endif // !_DEBUG

	// --------------------------------
	// Pixel Shader
	// --------------------------------

	ID3DBlob* shader_compile_errors_blob = nullptr;          // To get info about compilation

	// Compile Pixel Shader
	if (D3DCompileFromFile(pixel_path, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, shader_model, shader_flags, NULL, &m_psblob, &shader_compile_errors_blob) != S_OK)
	{
		if (shader_compile_errors_blob != nullptr)
		{
			OutputDebugStringA((char*)shader_compile_errors_blob->GetBufferPointer());
			JERROR(ErrorCode::ERR_SHADER_D3D11_PIXEL_COMPILATION, "%s", (char*)shader_compile_errors_blob->GetBufferPointer());
			shader_compile_errors_blob->Release();
		}
		else
		{
			JERROR(ErrorCode::ERR_SHADER_D3D11_PIXEL_COMPILATION, "Failed to compile Pixel Shader.");
		}
	}

	if (shader_compile_errors_blob != nullptr)
	{
		shader_compile_errors_blob->Release();
	}
}

void joj::D3D11Shader::compile_geometry_shader(const WCHAR* geometry_path, LPCSTR entry_point, LPCSTR shader_model)
{
	DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Let compiler insert debug information into the output code
	shader_flags |= D3DCOMPILE_DEBUG;

	// Disable optimizations
	shader_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;    // Compiler will not validate the generated code -> Recommended to use only with successfully compiled shaders
#endif // !_DEBUG

	// --------------------------------
	// Geometry Shader
	// --------------------------------

	ID3DBlob* shader_compile_errors_blob;          // To get info about compilation

	// Compile Vertex Shader
	if (D3DCompileFromFile(geometry_path, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, shader_model, shader_flags, NULL, &m_gsblob, &shader_compile_errors_blob) != S_OK)
	{
		OutputDebugStringA((char*)shader_compile_errors_blob->GetBufferPointer());
		JERROR(ErrorCode::ERR_SHADER_D3D11_GEOMETRY_COMPILATION, "%s", (char*)shader_compile_errors_blob->GetBufferPointer());
		shader_compile_errors_blob->Release();
	}

	if (shader_compile_errors_blob != nullptr)
	{
		shader_compile_errors_blob->Release();
	}
}

void joj::D3D11Shader::compile_compute_shader(const WCHAR* compute_path, LPCSTR entry_point, LPCSTR shader_model)
{
	DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Let compiler insert debug information into the output code
	shader_flags |= D3DCOMPILE_DEBUG;

	// Disable optimizations
	shader_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;    // Compiler will not validate the generated code -> Recommended to use only with successfully compiled shaders
#endif // !_DEBUG

	// --------------------------------
	// Compute Shader
	// --------------------------------

	ID3DBlob* shader_compile_errors_blob;          // To get info about compilation

	// Compile Vertex Shader
	if (D3DCompileFromFile(compute_path, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, shader_model, shader_flags, NULL, &m_csblob, &shader_compile_errors_blob) != S_OK)
	{
		OutputDebugStringA((char*)shader_compile_errors_blob->GetBufferPointer());
		JERROR(ErrorCode::ERR_SHADER_D3D11_COMPUTE_COMPILATION, "%s", (char*)shader_compile_errors_blob->GetBufferPointer());
		shader_compile_errors_blob->Release();
	}

	if (shader_compile_errors_blob != nullptr)
	{
		shader_compile_errors_blob->Release();
	}
}

joj::ErrorCode joj::D3D11Shader::create_vertex_shader(ID3D11Device* device)
{
	if (device->CreateVertexShader(
		m_vsblob->GetBufferPointer(),
		m_vsblob->GetBufferSize(),
		nullptr,
		&m_vertex_shader) != S_OK)
	{
		JERROR(ErrorCode::ERR_SHADER_D3D11_VERTEX_CREATION,
			"Failed to create Vertex Shader.");
		return ErrorCode::ERR_SHADER_D3D11_VERTEX_CREATION;
	}

	return ErrorCode::OK;
}

joj::ErrorCode joj::D3D11Shader::create_pixel_shader(ID3D11Device* device)
{
	if (device->CreatePixelShader(
		m_psblob->GetBufferPointer(),
		m_psblob->GetBufferSize(),
		nullptr,
		&m_pixel_shader) != S_OK)
	{
		JERROR(ErrorCode::ERR_SHADER_D3D11_PIXEL_CREATION,
			"Failed to create Pixel Shader.");
		return ErrorCode::ERR_SHADER_D3D11_PIXEL_CREATION;
	}

	return ErrorCode::OK;
}

void joj::D3D11Shader::bind_vertex_shader(ID3D11DeviceContext* device_context)
{
	device_context->VSSetShader(m_vertex_shader, nullptr, 0);
}

void joj::D3D11Shader::bind_pixel_shader(ID3D11DeviceContext* device_context)
{
	device_context->PSSetShader(m_pixel_shader, nullptr, 0);
}

#endif // JPLATFORM_WINDOWS