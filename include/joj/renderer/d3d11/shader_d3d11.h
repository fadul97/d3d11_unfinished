#ifndef JOJ_D3D11_SHADER_H
#define JOJ_D3D11_SHADER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>
#include "error_code.h"

namespace joj
{
	// TODO: Load up prebuilt .cso files instead
	class JAPI D3D11Shader
	{
	public:
		D3D11Shader();
		~D3D11Shader();

		// Compile from file
		void compile_vertex_shader(const WCHAR* vertex_path, LPCSTR entry_point, LPCSTR shader_model);
		// Compile from file
		void compile_pixel_shader(const WCHAR* pixel_path, LPCSTR entry_point, LPCSTR shader_model);

		void compile_geometry_shader(const WCHAR* geometry_path, LPCSTR entry_point, LPCSTR shader_model);
		void compile_compute_shader(const WCHAR* compute_path, LPCSTR entry_point, LPCSTR shader_model);

		ErrorCode create_vertex_shader(ID3D11Device* device);
		ErrorCode create_pixel_shader(ID3D11Device* device);

		void bind_vertex_shader(ID3D11DeviceContext* device_context);
		void bind_pixel_shader(ID3D11DeviceContext* device_context);

		ID3D11VertexShader*& get_vertex_shader();
		ID3D11PixelShader*& get_pixel_shader();
		ID3D11GeometryShader*& get_geometry_shader();
		ID3D11ComputeShader*& get_compute_shader();
		
		ID3DBlob* get_vsblob() const;
		ID3DBlob* get_psblob() const;
		ID3DBlob* get_gsblob() const;
		ID3DBlob* get_csblob() const;

	private:
		ID3D11VertexShader* m_vertex_shader;        // Manages Vertex Shader Program and control Vertex Shader Stage 
		ID3D11PixelShader* m_pixel_shader;          // Manages Pixel Shader Program and controls Pixel Shader Stage
		ID3D11GeometryShader* m_geometry_shader;    // Manages Geometry Shader Program and controls Geometry Shader Stage
		ID3D11ComputeShader* m_compute_shader;      // Manages Compute Shader Program and controls Compute Shader Stage

		ID3DBlob* m_vsblob;                         // Vertex shader
		ID3DBlob* m_psblob;                         // Pixel shader
		ID3DBlob* m_gsblob;                         // Geometry shader
		ID3DBlob* m_csblob;                         // Compute shader
	};

	inline ID3D11VertexShader*& D3D11Shader::get_vertex_shader()
	{ return m_vertex_shader; }
	
	inline ID3D11PixelShader*& D3D11Shader::get_pixel_shader()
	{ return m_pixel_shader; }

	inline ID3D11GeometryShader*& D3D11Shader::get_geometry_shader()
	{ return m_geometry_shader; }

	inline ID3D11ComputeShader*& D3D11Shader::get_compute_shader()
	{ return m_compute_shader; }

	inline ID3DBlob* D3D11Shader::get_vsblob() const
	{ return m_vsblob; }
	
	inline ID3DBlob* D3D11Shader::get_psblob() const
	{ return m_psblob; }

	inline ID3DBlob* D3D11Shader::get_gsblob() const
	{ return m_gsblob; }

	inline ID3DBlob* D3D11Shader::get_csblob() const
	{ return m_csblob; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_SHADER_H