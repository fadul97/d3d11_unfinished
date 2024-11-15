#include "renderer/d3d11/renderable_object_d3d11.h"

#if JPLATFORM_WINDOWS

joj::D3D11RenderableObject::D3D11RenderableObject()
{
    m_world = float4x4_identity();
    m_index_count = 0;
    m_index_location = 0;
    m_vertex_location = 0;
}

joj::D3D11RenderableObject::~D3D11RenderableObject()
{
}

#endif // JPLATFORM_WINDOWS