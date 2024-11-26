#ifndef JOJ_D3D11_VERTEX_H
#define JOJ_D3D11_VERTEX_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "math/jmath.h"
#include <d3d11.h>

namespace joj
{
	namespace Vertex
	{
		// Basic 32-byte vertex structure.
		struct Basic32
		{
			JFloat3 pos;
			JFloat3 normal;
			JFloat2 tex;
		};

		struct PosNormalTexTan
		{
			JFloat3 pos;
			JFloat3 normal;
			JFloat2 tex;
			JFloat4 tangentU;
		};

		static const D3D11_INPUT_ELEMENT_DESC pos_input_desc[1] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		static const D3D11_INPUT_ELEMENT_DESC basic32_input_desc[3] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		static const D3D11_INPUT_ELEMENT_DESC pos_normal_tex_tan_input_desc[4] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32,  D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_VERTEX_H