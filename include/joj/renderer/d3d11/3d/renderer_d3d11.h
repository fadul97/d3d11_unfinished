#ifndef JOJ_D3D11_RENDERER_H
#define JOJ_D3D11_RENDERER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "context_d3d11.h"

namespace joj
{
    enum class RasterizerFillMode { Solid, Wireframe };

    class JAPI D3D11Renderer
    {
    public:
        D3D11Renderer();
        ~D3D11Renderer();

        ErrorCode init(WindowData& window);
        void clear(f32 r = 0.23f, f32 g = 0.23f, f32 b = 0.23f, f32 a = 1.0f);
        void render();
        void swap_buffers();

        void set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY topology) const;
        void set_rasterizer_fill_mode(RasterizerFillMode mode);

        ID3D11Device* get_device() const;
        ID3D11DeviceContext* get_device_context() const;

        ID3D11DepthStencilView* get_depth_stencil_view() const;
        ID3D11RenderTargetView* get_render_target_view() const;

    private:
        D3D11Context* m_context;
        ID3D11Device* m_device;
        ID3D11DeviceContext* m_device_context;

        /**
         * @brief Pipeline  members
         *
         */

        b8 m_4xmsaa_enabled;
        u32 m_4xmsaa_quality;
        u32 m_buffer_count;
        u32 m_mip_levels;
        b8 m_vsync;                                             // Vertical sync 
        IDXGISwapChain* m_swapchain;                            // Swap chain
        ID3D11RenderTargetView* m_render_target_view;           // Backbuffer render target view
        ID3D11Texture2D* m_depth_stencil_buffer;                // Depth/Stencil buffer
        ID3D11DepthStencilView* m_depth_stencil_view;           // Depth/Stencil view
        D3D11_VIEWPORT m_viewport;                              // Viewport
        ID3D11BlendState* m_blend_state;                        // Color mix settings
        ID3D11RasterizerState* m_rasterizer_state_solid;        // Solid Rasterizer state
        ID3D11RasterizerState* m_rasterizer_state_wireframe;    // Wireframe Rasterizer state
    };

    inline ID3D11Device* D3D11Renderer::get_device() const
    { return m_device; }

    inline ID3D11DeviceContext* D3D11Renderer::get_device_context() const
    { return m_device_context; }

    inline ID3D11DepthStencilView* D3D11Renderer::get_depth_stencil_view() const
    { return m_depth_stencil_view; }

    inline ID3D11RenderTargetView* D3D11Renderer::get_render_target_view() const
    { return m_render_target_view; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_RENDERER_H