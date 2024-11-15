#ifndef JOJ_D3D11_SHADER_H
#define JOJ_D3D11_SHADER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>

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

		ID3D11VertexShader*& get_vertex_shader();
		ID3D11PixelShader*& get_pixel_shader();
		
		ID3DBlob* get_vsblob() const;
		ID3DBlob* get_psblob() const;

	private:
		ID3D11VertexShader* m_vertex_shader;  // Manages Vertex Shader Program and control Vertex Shader Stage 
		ID3D11PixelShader* m_pixel_shader;    // Manages Pixel Shader Program and controls Pixel Shader Stage
		ID3DBlob* m_vsblob;                   // Vertex shader
		ID3DBlob* m_psblob;                   // Pixel shader
	};

	inline ID3D11VertexShader*& D3D11Shader::get_vertex_shader()
	{ return m_vertex_shader; }
	
	inline ID3D11PixelShader*& D3D11Shader::get_pixel_shader()
	{ return m_pixel_shader; }

	inline ID3DBlob* D3D11Shader::get_vsblob() const
	{ return m_vsblob; }
	
	inline ID3DBlob* D3D11Shader::get_psblob() const
	{ return m_psblob; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_SHADER_H