#ifndef JOJ_D3D11_RENDERER_H
#define JOJ_D3D11_RENDERER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "context_d3d11.h"

namespace joj
{
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

        ID3D11Device* get_device() const;
        ID3D11DeviceContext* get_device_context() const;

    private:
        D3D11Context* m_context;
        ID3D11Device* m_device;
        ID3D11DeviceContext* m_device_context;

        /**
         * @brief Pipeline  members
         *
         */

        u32 m_antialiasing;                            // Number of samples for each pixel on the screen
        u32 m_quality;                                 // Antialiasing sampling quality
        b8 m_vsync;                                    // Vertical sync 
        IDXGISwapChain* m_swapchain;                   // Swap chain
        ID3D11RenderTargetView* m_render_target_view;  // Backbuffer render target view
        ID3D11DepthStencilView* m_depth_stencil_view;  // Depth/Stencil view
        D3D11_VIEWPORT m_viewport;                     // Viewport
        ID3D11BlendState* m_blend_state;               // Color mix settings
        ID3D11RasterizerState* m_rasterizer_state;     // Rasterizer state
    };

    inline ID3D11Device* D3D11Renderer::get_device() const
    { return m_device; }

    inline ID3D11DeviceContext* D3D11Renderer::get_device_context() const
    { return m_device_context; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_RENDERER_H