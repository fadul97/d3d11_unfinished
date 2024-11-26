#ifndef JOJ_D3D11_MESH_H
#define JOJ_D3D11_MESH_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>
#include <vector>
#include "vertex_buffer_d3d11.h"
#include "index_buffer_d3d11.h"
#include "logger.h"

namespace joj
{
    struct D3D11Submesh
    {
        D3D11Submesh()
            : id(-1), vertex_start(0), vertex_count(0),
            face_start(0), face_count(0)
        {
        }

        u32 id;
        u32 vertex_start;
        u32 vertex_count;
        u32 face_start;
        u32 face_count;
    };

    class JAPI D3D11Mesh
    {
    public:
        D3D11Mesh();
        ~D3D11Mesh();

        D3D11Mesh(const D3D11Mesh& rhs) = delete;
        D3D11Mesh& operator=(const D3D11Mesh& rhs) = delete;

        template<typename VertexType>
        ErrorCode set_vertices(ID3D11Device* device, const VertexType* vertices, u32 count);

        ErrorCode set_indices(ID3D11Device* device, const u16* indices, u32 count);

        void set_submeshes(std::vector<D3D11Submesh>& submeshes);

        void draw(ID3D11DeviceContext* dc, u32 submesh_id);

    private:
        D3D11VertexBuffer m_vb;
        D3D11IndexBuffer m_ib;

        DXGI_FORMAT m_index_buffer_format;    // Always 16-bit
        u32 m_vertex_stride;

        std::vector<D3D11Submesh> m_submeshes;
    };

    template<typename VertexType>
    inline ErrorCode D3D11Mesh::set_vertices(ID3D11Device* device, const VertexType* vertices, u32 count)
    {
        if (m_vb.is_filled())
            m_vb.cleanup();

        m_vertex_stride = sizeof(VertexType);

        m_vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(VertexType) * count, vertices);
        
        if (vertices != nullptr)
        {
            if (device->CreateBuffer(
                m_vb.get_buffer_desc(),
                m_vb.get_subdata(),
                &m_vb.get_buffer()) != S_OK)
            {
                JERROR(ErrorCode::ERR_RENDERER_D3D11_VERTEX_BUFFER_CREATION,
                    "Failed to create Vertex Buffer.");

                return ErrorCode::ERR_RENDERER_D3D11_VERTEX_BUFFER_CREATION;
            }
        }
        else
        {
            if (device->CreateBuffer(
                m_vb.get_buffer_desc(),
                nullptr,
                &m_vb.get_buffer()) != S_OK)
            {
                JERROR(ErrorCode::ERR_RENDERER_D3D11_VERTEX_BUFFER_CREATION,
                    "Failed to create Vertex Buffer.");

                return ErrorCode::ERR_RENDERER_D3D11_VERTEX_BUFFER_CREATION;
            }
        }

        return ErrorCode::OK;
    }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_MESH_H