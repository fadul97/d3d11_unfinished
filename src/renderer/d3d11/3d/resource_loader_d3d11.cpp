#include "renderer/d3d11/3d/resources_loader_d3d11.h"

#if JPLATFORM_WINDOWS

#include <fstream>

namespace joj
{
	namespace D3D11ResourceLoader
	{
		void read_materials(std::ifstream& fin, u32 num_materials, std::vector<M3dMaterial>& mats);
		void read_submeshes(std::ifstream& fin, u32 num_submeshes, std::vector<D3D11Submesh>& subsets);
		void read_vertices(std::ifstream& fin, u32 num_vertices, std::vector<Vertex::PosNormalTexTan>& vertices);
		void read_triangles(std::ifstream& fin, u32 num_triangles, std::vector<USHORT>& indices);
	}
}

b8 joj::D3D11ResourceLoader::load_m3d(const std::string& filename,
	std::vector<joj::Vertex::PosNormalTexTan>& vertices,
	std::vector<USHORT>& indices,
	std::vector<D3D11Submesh>& submeshes,
	std::vector<M3dMaterial>& mats)
{
	std::ifstream fin(filename);

	if (fin)
	{
		u32 num_materials = 0;
		u32 num_vertices = 0;
		u32 num_triangles = 0;
		u32 num_bones = 0;
		u32 num_animation_clips = 0;

		std::string ignore;

		fin >> ignore; // file header text
		fin >> ignore >> num_materials;
		fin >> ignore >> num_vertices;
		fin >> ignore >> num_triangles;
		fin >> ignore >> num_bones;
		fin >> ignore >> num_animation_clips;

		read_materials(fin, num_materials, mats);
		read_submeshes(fin, num_materials, submeshes);
		read_vertices(fin, num_vertices, vertices);
		read_triangles(fin, num_triangles, indices);

		return true;
	}

	return false;
}

void joj::D3D11ResourceLoader::read_materials(std::ifstream& fin, u32 num_materials, std::vector<M3dMaterial>& mats)
{
	std::string ignore;
	mats.resize(num_materials);

	std::string diffuseMapName;
	std::string normalMapName;

	fin >> ignore; // materials header text
	for (u32 i = 0; i < num_materials; ++i)
	{
		fin >> ignore >> mats[i].mat.ambient.x >> mats[i].mat.ambient.y >> mats[i].mat.ambient.z;
		fin >> ignore >> mats[i].mat.diffuse.x >> mats[i].mat.diffuse.y >> mats[i].mat.diffuse.z;
		fin >> ignore >> mats[i].mat.specular.x >> mats[i].mat.specular.y >> mats[i].mat.specular.z;
		fin >> ignore >> mats[i].mat.specular.w;
		fin >> ignore >> mats[i].mat.Reflect.x >> mats[i].mat.Reflect.y >> mats[i].mat.Reflect.z;
		fin >> ignore >> mats[i].alpha_clip;
		fin >> ignore >> mats[i].effect_type_name;
		fin >> ignore >> diffuseMapName;
		fin >> ignore >> normalMapName;

		mats[i].diffuse_map_name.resize(diffuseMapName.size(), ' ');
		mats[i].normal_map_name.resize(normalMapName.size(), ' ');
		std::copy(diffuseMapName.begin(), diffuseMapName.end(), mats[i].diffuse_map_name.begin());
		std::copy(normalMapName.begin(), normalMapName.end(), mats[i].normal_map_name.begin());
	}
}

void joj::D3D11ResourceLoader::read_submeshes(std::ifstream& fin, u32 num_submeshes, std::vector<D3D11Submesh>& subsets)
{
	std::string ignore;
	subsets.resize(num_submeshes);

	fin >> ignore; // subset header text
	for (u32 i = 0; i < num_submeshes; ++i)
	{
		fin >> ignore >> subsets[i].id;
		fin >> ignore >> subsets[i].vertex_start;
		fin >> ignore >> subsets[i].vertex_count;
		fin >> ignore >> subsets[i].face_start;
		fin >> ignore >> subsets[i].face_count;
	}
}

void joj::D3D11ResourceLoader::read_vertices(std::ifstream& fin, u32 num_vertices, std::vector<Vertex::PosNormalTexTan>& vertices)
{
	std::string ignore;
	vertices.resize(num_vertices);

	fin >> ignore; // vertices header text
	for (u32 i = 0; i < num_vertices; ++i)
	{
		fin >> ignore >> vertices[i].pos.x >> vertices[i].pos.y >> vertices[i].pos.z;
		fin >> ignore >> vertices[i].tangentU.x >> vertices[i].tangentU.y >> vertices[i].tangentU.z >> vertices[i].tangentU.w;
		fin >> ignore >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;
		fin >> ignore >> vertices[i].tex.x >> vertices[i].tex.y;
	}
}

void joj::D3D11ResourceLoader::read_triangles(std::ifstream& fin, u32 num_triangles, std::vector<u16>& indices)
{
	std::string ignore;
	indices.resize(num_triangles * 3);

	fin >> ignore; // triangles header text
	for (u32 i = 0; i < num_triangles; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}
}

#endif // JPLATFORM_WINDOWS