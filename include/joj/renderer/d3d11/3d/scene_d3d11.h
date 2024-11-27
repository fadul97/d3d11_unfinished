#ifndef JOJ_D3D11_SCENE_H
#define JOJ_D3D11_SCENE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <vector>
#include <memory>
#include "renderable_object_d3d11.h"
#include "systems/camera/free_camera.h"
#include "renderer/d3d11/constant_buffer_d3d11.h"
#include "cbuffer_structs_d3d11.h"

namespace joj
{
    class JAPI D3D11Scene
    {
    public:
        D3D11Scene();
        ~D3D11Scene();

        D3D11Scene(const D3D11Scene&) = delete;
        D3D11Scene& operator=(const D3D11Scene&) = delete;

        u32 get_objects_count() const;

        void add_renderable_object(std::unique_ptr<D3D11RenderableObject> obj);

        void update(
            const f32 dt,
            const FreeCamera* camera,
            const JFloat4x4 proj_float4x4,
            D3D11ConstantBuffer& constant_buffer
        );

        void draw();

    private:
        std::vector<std::unique_ptr<D3D11RenderableObject>> m_objects;
    };

    inline u32 D3D11Scene::get_objects_count() const
    { return m_objects.size(); }

    inline void D3D11Scene::add_renderable_object(std::unique_ptr<D3D11RenderableObject> obj)
    { m_objects.push_back(std::move(obj)); }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_SCENE_H