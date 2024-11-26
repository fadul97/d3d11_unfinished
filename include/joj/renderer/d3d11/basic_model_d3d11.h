#ifndef JOJ_D3D11_BASIC_MODEL_H
#define JOJ_D3D11_BASIC_MODEL_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>
#include "texture_manager_d3d11.h"
#include <string>
#include <vector>
#include "systems/material/material.h"
#include "vertex_d3d11.h"
#include "mesh_d3d11.h"

namespace joj
{
    class JAPI D3D11BasicModel
    {
    public:
        D3D11BasicModel(ID3D11Device* device,
            D3D11TextureManager& tex_mgr,
            const std::string& model_filename,
            const std::wstring& texture_path);
        ~D3D11BasicModel();

        u32 m_submesh_count;
    
        std::vector<SimpleMaterial> m_mat;
        std::vector<ID3D11ShaderResourceView*> m_diffuse_map_SRV;
        std::vector<ID3D11ShaderResourceView*> m_normal_map_SRV;

        // Keep CPU copies of the mesh data to read from.  
        std::vector<Vertex::PosNormalTexTan> m_vertices;
        std::vector<u16> m_indices;
        std::vector<D3D11Submesh> m_submeshes;

        D3D11Mesh m_model_mesh;
    private:
    };

    struct BasicModelInstance
    {
        D3D11BasicModel* model;
        JFloat4x4 world;
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_BASIC_MODEL_H