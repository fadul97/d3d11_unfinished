#include "renderer/d3d11/texture_manager_d3d11.h"

#if JPLATFORM_WINDOWS

#include "renderer/d3d11/DDSTextureLoader11.h"
#include "logger.h"

joj::D3D11TextureManager::D3D11TextureManager()
	: m_device(nullptr)
{
}

joj::D3D11TextureManager::~D3D11TextureManager()
{
	for (auto it = m_texture_SRV.begin(); it != m_texture_SRV.end(); ++it)
	{
		it->second->Release();
	}

	m_texture_SRV.clear();
}

joj::ErrorCode joj::D3D11TextureManager::init(ID3D11Device* device)
{
	m_device = device;

	return ErrorCode::OK;
}

ID3D11ShaderResourceView* joj::D3D11TextureManager::create_texture(std::wstring filename)
{
	ID3D11ShaderResourceView* srv = nullptr;

	const wchar_t* filename_cstr = filename.c_str();

	// Does it already exist?
	if (m_texture_SRV.find(filename) != m_texture_SRV.end())
	{
		srv = m_texture_SRV[filename];
	}
	else
	{
		if (DirectX::CreateDDSTextureFromFile(
			m_device,
			filename_cstr,
			nullptr,
			&srv) != S_OK)
		{
			JERROR(ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION,
				"Failed to create Shader Resource View for '%ls'", filename_cstr);

			return nullptr;
		}

		m_texture_SRV[filename] = srv;
	}

	return srv;
}

#endif // JPLATFORM_WINDOWS