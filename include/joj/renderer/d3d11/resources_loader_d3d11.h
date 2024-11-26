#ifndef JOJ_D3D11_RESOURCES_LOADER_H
#define JOJ_D3D11_RESOURCES_LOADER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "systems/material/material.h";
#include <vector>
#include <string>
#include "mesh_d3d11.h"
#include "vertex_d3d11.h"

namespace joj
{
	struct M3dMaterial
	{
		SimpleMaterial mat;
		b8 alpha_clip;
		std::string effect_type_name;
		std::wstring diffuse_map_name;
		std::wstring normal_map_name;
	};

	namespace D3D11ResourceLoader
	{
		bool load_m3d(const std::string& filename,
			std::vector<Vertex::PosNormalTexTan>& vertices,
			std::vector<u16>& indices,
			std::vector<D3D11Submesh>& submeshes,
			std::vector<M3dMaterial>& mats);
	}
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_RESOURCES_LOADER_H