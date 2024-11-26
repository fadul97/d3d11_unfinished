#include "renderer/d3d11/basic_model_d3d11.h"

#if JPLATFORM_WINDOWS

#include "renderer/d3d11/resources_loader_d3d11.h"

joj::D3D11BasicModel::D3D11BasicModel(ID3D11Device* device,
	D3D11TextureManager& tex_mgr,
	const std::string& model_filename,
	const std::wstring& texture_path)
{
	std::vector<M3dMaterial> mats;
	D3D11ResourceLoader::load_m3d(model_filename, m_vertices, m_indices, m_submeshes, mats);

	m_model_mesh.set_vertices(device, &m_vertices[0], m_vertices.size());
	m_model_mesh.set_indices(device, &m_indices[0], m_indices.size());
	m_model_mesh.set_submeshes(m_submeshes);

	m_submesh_count = mats.size();

	for (u32 i = 0; i < m_submesh_count; ++i)
	{
		m_mat.push_back(mats[i].mat);

		ID3D11ShaderResourceView* diffuseMapSRV = tex_mgr.create_texture(texture_path + mats[i].diffuse_map_name);
		m_diffuse_map_SRV.push_back(diffuseMapSRV);

		ID3D11ShaderResourceView* normalMapSRV = tex_mgr.create_texture(texture_path + mats[i].normal_map_name);
		m_normal_map_SRV.push_back(normalMapSRV);
	}
}

joj::D3D11BasicModel::~D3D11BasicModel()
{
}

#endif // JPLATFORM_WINDOWS