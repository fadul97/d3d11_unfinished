#include "renderer/d3d11/3d/mesh_d3d11.h"

#if JPLATFORM_WINDOWS

joj::D3D11Mesh::D3D11Mesh()
{
    m_index_buffer_format = DXGI_FORMAT_R16_UINT;
    m_vertex_stride = 0;
}

joj::D3D11Mesh::~D3D11Mesh()
{

}

joj::ErrorCode joj::D3D11Mesh::set_indices(ID3D11Device* device, const u16* indices, u32 count)
{
    m_ib.setup(sizeof(u16) * count, indices);

    if (device->CreateBuffer(
        m_ib.get_buffer_desc(),
        m_ib.get_subdata(),
        &m_ib.get_buffer()) != S_OK)
    {
        JERROR(ErrorCode::ERR_RENDERER_D3D11_INDEX_BUFFER_CREATION,
            "Failed to create Index Buffer.");

        return ErrorCode::ERR_RENDERER_D3D11_INDEX_BUFFER_CREATION;
    }

    return ErrorCode::OK;
}

void joj::D3D11Mesh::set_submeshes(std::vector<D3D11Submesh>& submeshes)
{
    m_submeshes = submeshes;
}

void joj::D3D11Mesh::draw(ID3D11DeviceContext* dc, u32 submesh_id)
{
    u32 offset = 0;

    dc->IASetVertexBuffers(0, 1, &m_vb.get_buffer(), &m_vertex_stride, &offset);
    dc->IASetIndexBuffer(m_ib.get_buffer(), m_index_buffer_format, 0);

    dc->DrawIndexed(
        m_submeshes[submesh_id].face_count * 3,
        m_submeshes[submesh_id].face_start * 3,
        0);
}

#endif // JPLATFORM_WINDOWS