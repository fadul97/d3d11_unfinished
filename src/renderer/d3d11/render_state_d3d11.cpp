#include "renderer/d3d11/render_state_d3d11.h"

#if JPLATFORM_WINDOWS

#include "engine.h"
#include "logger.h"

joj::D3D11RenderState::D3D11RenderState()
{
    m_wireframe_RS = nullptr;
    m_no_cull_RS = nullptr;
    m_cull_clock_wise_RS = nullptr;
    m_alpha_to_coverage_BS = nullptr;
    m_transparent_BS = nullptr;
    m_no_render_target_write_BS = nullptr;
    m_mark_mirror_DSS = nullptr;
    m_draw_reflection_DSS = nullptr;
    m_no_double_blend_DSS = nullptr;
}

joj::D3D11RenderState::~D3D11RenderState()
{
    // Release Wireframe Rasterizer
    if (m_wireframe_RS)
    {
        m_wireframe_RS->Release();
        m_wireframe_RS = nullptr;
    }
    
    // Release No Cull Rasterizer
    if (m_no_cull_RS)
    {
        m_no_cull_RS->Release();
        m_no_cull_RS = nullptr;
    }
    
    // Relase No Cull Clockwise Rasterizer
    if (m_cull_clock_wise_RS)
    {
        m_cull_clock_wise_RS->Release();
        m_cull_clock_wise_RS = nullptr;
    }
    
    // Release Alpha to Coverage Blend State
    if (m_alpha_to_coverage_BS)
    {
        m_alpha_to_coverage_BS->Release();
        m_alpha_to_coverage_BS = nullptr;
    }
    
    // Release Transparent Blend State
    if (m_transparent_BS)
    {
        m_transparent_BS->Release();
        m_transparent_BS = nullptr;
    }
    
    // Release No Render Target Write Blend State
    if (m_no_render_target_write_BS)
    {
        m_no_render_target_write_BS->Release();
        m_no_render_target_write_BS = nullptr;
    }
    
    // Release Mark Mirror Depth Stencil State
    if (m_mark_mirror_DSS)
    {
        m_mark_mirror_DSS->Release();
        m_mark_mirror_DSS = nullptr;
    }
    
    // Release Draw Reflection Depth Stencil State
    if (m_draw_reflection_DSS)
    {
        m_draw_reflection_DSS->Release();
        m_draw_reflection_DSS = nullptr;
    }

    // Release No Double Blend Depth Stencil State
    if (m_no_double_blend_DSS)
    {
        m_no_double_blend_DSS->Release();
        m_no_double_blend_DSS = nullptr;
    }
}

joj::ErrorCode joj::D3D11RenderState::create_rasterizer_state(RasterizerStateOption rasterizer_state_option)
{
    switch (rasterizer_state_option)
    {
    case RasterizerStateOption::Wireframe:
    {
        D3D11_RASTERIZER_DESC wireframe_desc;
        ZeroMemory(&wireframe_desc, sizeof(D3D11_RASTERIZER_DESC));
        wireframe_desc.FillMode = D3D11_FILL_WIREFRAME;
        wireframe_desc.CullMode = D3D11_CULL_BACK;
        wireframe_desc.FrontCounterClockwise = false;
        wireframe_desc.DepthClipEnable = true;

        if (Engine::s_renderer->get_device()->CreateRasterizerState(
            &wireframe_desc, &m_wireframe_RS) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDER_STATE_D3D11_RASTERIZER_STATE_CREATION,
                "Failed to create RasterizerState.");
            return ErrorCode::ERR_RENDER_STATE_D3D11_RASTERIZER_STATE_CREATION;
        }
    }
        break;
    case RasterizerStateOption::NoCull:
    {
        D3D11_RASTERIZER_DESC no_cull_desc;
        ZeroMemory(&no_cull_desc, sizeof(D3D11_RASTERIZER_DESC));
        no_cull_desc.FillMode = D3D11_FILL_SOLID;
        no_cull_desc.CullMode = D3D11_CULL_NONE;
        no_cull_desc.FrontCounterClockwise = false;
        no_cull_desc.DepthClipEnable = true;

        // Create rasterizer state
        if (Engine::s_renderer->get_device()->CreateRasterizerState(
            &no_cull_desc, &m_no_cull_RS) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDER_STATE_D3D11_RASTERIZER_STATE_CREATION,
                "Failed to create RasterizerState.");
            return ErrorCode::ERR_RENDER_STATE_D3D11_RASTERIZER_STATE_CREATION;
        }
    }
        break;
    case RasterizerStateOption::CullClockwise:
    {
        D3D11_RASTERIZER_DESC cull_clock_wise_desc;
        ZeroMemory(&cull_clock_wise_desc, sizeof(D3D11_RASTERIZER_DESC));
        cull_clock_wise_desc.FillMode = D3D11_FILL_SOLID;
        cull_clock_wise_desc.CullMode = D3D11_CULL_BACK;
        cull_clock_wise_desc.FrontCounterClockwise = true;
        cull_clock_wise_desc.DepthClipEnable = true;

        // Create rasterizer state
        if (Engine::s_renderer->get_device()->CreateRasterizerState(
            &cull_clock_wise_desc, &m_cull_clock_wise_RS) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDER_STATE_D3D11_RASTERIZER_STATE_CREATION,
                "Failed to create RasterizerState.");
            return ErrorCode::ERR_RENDER_STATE_D3D11_RASTERIZER_STATE_CREATION;
        }
    }
        break;
    default:
        break;
    }

    return ErrorCode::OK;
}

joj::ErrorCode joj::D3D11RenderState::create_blend_state(BlendStateOption blend_state_option)
{
    switch (blend_state_option)
    {
    case BlendStateOption::AlphaToCoverage:
    {
        D3D11_BLEND_DESC alpha_to_coverage_desc = { 0 };
        alpha_to_coverage_desc.AlphaToCoverageEnable = true;
        alpha_to_coverage_desc.IndependentBlendEnable = false;
        alpha_to_coverage_desc.RenderTarget[0].BlendEnable = false;
        alpha_to_coverage_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        // Create rasterizer state
        if (Engine::s_renderer->get_device()->CreateBlendState(
            &alpha_to_coverage_desc, &m_alpha_to_coverage_BS) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDER_STATE_D3D11_BLEND_STATE_CREATION,
                "Failed to create Blend State.");
            return ErrorCode::ERR_RENDER_STATE_D3D11_BLEND_STATE_CREATION;
        }
    }
        break;
    case BlendStateOption::Transparent:
    {
        D3D11_BLEND_DESC transparent_desc = { 0 };
        transparent_desc.AlphaToCoverageEnable = false;
        transparent_desc.IndependentBlendEnable = false;

        transparent_desc.RenderTarget[0].BlendEnable = true;
        transparent_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        transparent_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        transparent_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        transparent_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        transparent_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        transparent_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        transparent_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        // Create rasterizer state
        if (Engine::s_renderer->get_device()->CreateBlendState(
            &transparent_desc, &m_transparent_BS) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDER_STATE_D3D11_BLEND_STATE_CREATION,
                "Failed to create Blend State.");
            return ErrorCode::ERR_RENDER_STATE_D3D11_BLEND_STATE_CREATION;
        }
    }
        break;
    case BlendStateOption::NoRenderTargetWrite:
    {
        D3D11_BLEND_DESC no_render_target_write_desc = { 0 };
        no_render_target_write_desc.AlphaToCoverageEnable = false;
        no_render_target_write_desc.IndependentBlendEnable = false;

        no_render_target_write_desc.RenderTarget[0].BlendEnable = false;
        no_render_target_write_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        no_render_target_write_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
        no_render_target_write_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        no_render_target_write_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        no_render_target_write_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        no_render_target_write_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        no_render_target_write_desc.RenderTarget[0].RenderTargetWriteMask = 0;

        // Create blend state
        if (Engine::s_renderer->get_device()->CreateBlendState(
            &no_render_target_write_desc, &m_no_render_target_write_BS) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDER_STATE_D3D11_BLEND_STATE_CREATION,
                "Failed to create Blend State.");
            return ErrorCode::ERR_RENDER_STATE_D3D11_BLEND_STATE_CREATION;
        }
    }
        break;
    default:
        break;
    }

    return ErrorCode::OK;
}

joj::ErrorCode joj::D3D11RenderState::create_depthstencil_state(DepthStencilStateOption depthstencil_state_option)
{
    switch (depthstencil_state_option)
    {
    case DepthStencilStateOption::MarkMirror:
    {
        D3D11_DEPTH_STENCIL_DESC mark_mirror_desc = { 0 };
        mark_mirror_desc.DepthEnable = true;
        mark_mirror_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        mark_mirror_desc.DepthFunc = D3D11_COMPARISON_LESS;
        mark_mirror_desc.StencilEnable = true;
        mark_mirror_desc.StencilReadMask = 0xff;
        mark_mirror_desc.StencilWriteMask = 0xff;

        mark_mirror_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        mark_mirror_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        mark_mirror_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        mark_mirror_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // We are not rendering backfacing polygons, so these settings do not matter.
        mark_mirror_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        mark_mirror_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        mark_mirror_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        mark_mirror_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Create depth stencil
        if (Engine::s_renderer->get_device()->CreateDepthStencilState(
            &mark_mirror_desc, &m_mark_mirror_DSS) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDER_STATE_D3D11_DEPTHSTENCIL_STATE_CREATION,
                "Failed to create Depth Stencil State.");
            return ErrorCode::ERR_RENDER_STATE_D3D11_DEPTHSTENCIL_STATE_CREATION;
        }
    }
        break;
    case DepthStencilStateOption::Reflection:
    {
        D3D11_DEPTH_STENCIL_DESC draw_reflection_desc = { 0 };
        draw_reflection_desc.DepthEnable = true;
        draw_reflection_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        draw_reflection_desc.DepthFunc = D3D11_COMPARISON_LESS;
        draw_reflection_desc.StencilEnable = true;
        draw_reflection_desc.StencilReadMask = 0xff;
        draw_reflection_desc.StencilWriteMask = 0xff;

        draw_reflection_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        draw_reflection_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        draw_reflection_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        draw_reflection_desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

        // We are not rendering backfacing polygons, so these settings do not matter.
        draw_reflection_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        draw_reflection_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        draw_reflection_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        draw_reflection_desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

        // Create depth stencil
        if (Engine::s_renderer->get_device()->CreateDepthStencilState(
            &draw_reflection_desc, &m_draw_reflection_DSS) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDER_STATE_D3D11_DEPTHSTENCIL_STATE_CREATION,
                "Failed to create Depth Stencil State.");
            return ErrorCode::ERR_RENDER_STATE_D3D11_DEPTHSTENCIL_STATE_CREATION;
        }
    }
        break;
    case DepthStencilStateOption::NoDoubleBlend:
    {
        D3D11_DEPTH_STENCIL_DESC no_double_blend_desc = { 0 };
        no_double_blend_desc.DepthEnable = true;
        no_double_blend_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        no_double_blend_desc.DepthFunc = D3D11_COMPARISON_LESS;
        no_double_blend_desc.StencilEnable = true;
        no_double_blend_desc.StencilReadMask = 0xff;
        no_double_blend_desc.StencilWriteMask = 0xff;

        no_double_blend_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        no_double_blend_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        no_double_blend_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
        no_double_blend_desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

        // We are not rendering backfacing polygons, so these settings do not matter.
        no_double_blend_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        no_double_blend_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        no_double_blend_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
        no_double_blend_desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

        // Create depth stencil
        if (Engine::s_renderer->get_device()->CreateDepthStencilState(
            &no_double_blend_desc, &m_no_double_blend_DSS) != S_OK)
        {
            JERROR(ErrorCode::ERR_RENDER_STATE_D3D11_DEPTHSTENCIL_STATE_CREATION,
                "Failed to create Depth Stencil State.");
            return ErrorCode::ERR_RENDER_STATE_D3D11_DEPTHSTENCIL_STATE_CREATION;
        }
    }
        break;
    default:
        break;
    }

    return ErrorCode::OK;
}

void joj::D3D11RenderState::set_rasterizer_state(RasterizerStateOption rasterizer_state_option)
{
    switch (rasterizer_state_option)
    {
    case RasterizerStateOption::Wireframe:
        Engine::s_renderer->get_device_context()->RSSetState(m_wireframe_RS);
        break;
    case RasterizerStateOption::NoCull:
        Engine::s_renderer->get_device_context()->RSSetState(m_no_cull_RS);
        break;
    case RasterizerStateOption::CullClockwise:
        Engine::s_renderer->get_device_context()->RSSetState(m_cull_clock_wise_RS);
        break;
    case RasterizerStateOption::None:
        Engine::s_renderer->get_device_context()->RSSetState(nullptr);
        break;
    default:
        break;
    }
}

void joj::D3D11RenderState::set_blend_state(BlendStateOption blend_state_option, f32 blend_factor[4])
{
    switch (blend_state_option)
    {
    case BlendStateOption::AlphaToCoverage:
        joj::Engine::s_renderer->get_device_context()->OMSetBlendState(m_alpha_to_coverage_BS, blend_factor, 0xffffffff);
        break;
    case BlendStateOption::Transparent:
        joj::Engine::s_renderer->get_device_context()->OMSetBlendState(m_transparent_BS, blend_factor, 0xffffffff);
        break;
    case BlendStateOption::NoRenderTargetWrite:
        joj::Engine::s_renderer->get_device_context()->OMSetBlendState(m_no_render_target_write_BS, blend_factor, 0xffffffff);
        break;
    case BlendStateOption::None:
        joj::Engine::s_renderer->get_device_context()->OMSetBlendState(nullptr, blend_factor, 0xffffffff);
        break;
    default:
        break;
    }
}

void joj::D3D11RenderState::set_depthstencil_state(DepthStencilStateOption depthstencil_state_option)
{
    switch (depthstencil_state_option)
    {
    case DepthStencilStateOption::MarkMirror:
        break;
    case DepthStencilStateOption::Reflection:
        break;
    case DepthStencilStateOption::NoDoubleBlend:
        break;
    case DepthStencilStateOption::None:
        break;
    default:
        break;
    }
}


#endif // JPLATFORM_WINDOWS