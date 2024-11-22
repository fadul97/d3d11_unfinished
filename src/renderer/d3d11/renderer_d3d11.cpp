#include "renderer/d3d11/renderer_d3d11.h"

#if JPLATFORM_WINDOWS

#include "logger.h"
#include <DirectXColors.h>
#include <d3dcompiler.h>

joj::D3D11Renderer::D3D11Renderer()
{
	m_context = nullptr;
	m_device = nullptr;
	m_device_context = nullptr;

	m_4xmsaa_enabled = true;                   // Yes antialising
	m_4xmsaa_quality = 0;                      // Default quality
	m_buffer_count = 2;                        // 2 buffers: Back and Front
	m_mip_levels = 1;                          // Number of mip levels
	m_vsync = false;                           // No vertical sync
	m_swapchain = nullptr;                     // Swap chain
	m_render_target_view = nullptr;            // Backbuffer render target view
	m_depth_stencil_buffer = nullptr;          // Depth/Stencil buffer
	m_depth_stencil_view = nullptr;            // Depth/Stencil view
	m_viewport = { 0 };                        // Viewport
	m_blend_state = nullptr;                   // Color mix settings
	m_rasterizer_state_solid = nullptr;        // Solid Rasterizer state
	m_rasterizer_state_wireframe = nullptr;    // Wireframe Rasterizer state
}

joj::D3D11Renderer::~D3D11Renderer()
{
	// Release wireframe rasterizer state
	if (m_rasterizer_state_wireframe)
	{
		m_rasterizer_state_wireframe->Release();
		m_rasterizer_state_wireframe = nullptr;
	}

	// Release solid rasterizer state
	if (m_rasterizer_state_solid)
	{
		m_rasterizer_state_solid->Release();
		m_rasterizer_state_solid = nullptr;
	}

	// Release blend state
	if (m_blend_state)
	{
		m_blend_state->Release();
		m_blend_state = nullptr;
	}

	// Release depth stencil view
	if (m_depth_stencil_view)
	{
		m_depth_stencil_view->Release();
		m_depth_stencil_view = nullptr;
	}

	// Release depth stencil buffer
	if (m_depth_stencil_buffer)
	{
		m_depth_stencil_buffer->Release();
		m_depth_stencil_buffer = nullptr;
	}

	// Release render target view
	if (m_render_target_view)
	{
		m_render_target_view->Release();
		m_render_target_view = nullptr;
	}

	// Release swap chain
	if (m_swapchain)
	{
		// Direct3D is unable to close when full screen
		m_swapchain->SetFullscreenState(false, NULL);
		m_swapchain->Release();
		m_swapchain = nullptr;
	}

	delete m_context;
}

joj::ErrorCode joj::D3D11Renderer::init(WindowData& window)
{
	// Initialize D3D11Context
	m_context = new D3D11Context();
	if (m_context->create() != ErrorCode::OK)
	{
		JFATAL(ErrorCode::ERR_D3D11CONTEXT_CREATION, "Failed to create D3D11Context.");
		return ErrorCode::ERR_D3D11CONTEXT_CREATION;
	}

	// Get pointers to D3D11 Device and D3D11 Device Context
	m_device = m_context->get_device();
	m_device_context = m_context->get_device_context();

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	if (m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xmsaa_quality) != S_OK)
	{
		// TODO: Better ErrorCode
		JERROR(ErrorCode::FAILED, "Failed to check multi sample quality levels.");
		return ErrorCode::FAILED;
	}
	else
	{
		JDEBUG("MSAA Quality: %d", m_4xmsaa_quality);
	}

	if (m_4xmsaa_quality <= 0)
	{
		// TODO: Better ErrorCode
		JERROR(ErrorCode::FAILED, "MSAA Quality is too low.");
		return ErrorCode::FAILED;
	}

	// ------------------------------------------------------------------------------------------------------
	//                                          PIPELINE SETUP
	// ------------------------------------------------------------------------------------------------------

	// ---------------------------------------------------
	// SwapChain
	// ---------------------------------------------------

	// Describe Swap Chain
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = { 0 };
	swap_chain_desc.BufferDesc.Width = static_cast<u32>(window.width);                      // Back buffer width
	swap_chain_desc.BufferDesc.Height = static_cast<u32>(window.height);                    // Back buffer height
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;                                  // Refresh rate in hertz 
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;                                 // Denominator is an int
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                         // Color format - RGBA 8 bits
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;     // Default value for Flags
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                     // Default mode for scaling

	// Using 4x MSAA
	if (m_4xmsaa_enabled)
	{
		swap_chain_desc.SampleDesc.Count = 4;                                               // Samples per pixel (antialiasing)
		swap_chain_desc.SampleDesc.Quality = m_4xmsaa_quality - 1;                          // Level of image quality
	}
	// No MSAA
	else
	{
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
	}

	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                          // Use surface as Render Target

	// Check value
	swap_chain_desc.BufferCount = m_buffer_count;                                           // Number of buffers (Front + Back)

	swap_chain_desc.OutputWindow = window.handle;                                           // Window ID
	swap_chain_desc.Windowed = (window.window_mode == joj::WindowMode::Windowed);           // Fullscreen or windowed 
	
	// Check values

	// TODO: swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD?
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                                  // Discard surface after presenting
	swap_chain_desc.Flags = 0;                                                              // Use Back buffer size for Fullscreen

	// Create Swap Chain
	if (m_context->get_factory()->CreateSwapChain(m_device, &swap_chain_desc, &m_swapchain) != S_OK)
	{
		JFATAL(ErrorCode::ERR_SWAPCHAIN_D311_CREATION, "Failed to create SwapChain.");
		return ErrorCode::ERR_SWAPCHAIN_D311_CREATION;
	}

	// ---------------------------------------------------
	// Render Target View
	// ---------------------------------------------------

	// Get backbuffer surface of a Swap Chain
	ID3D11Texture2D* backbuffer = nullptr;
	if (m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer)) != S_OK)
	{
		JFATAL(ErrorCode::ERR_SWAPCHAIN_D3D11_GET_BACKBUFFER, "Failed to Get backbuffer surface of a Swap Chain.");
		return ErrorCode::ERR_SWAPCHAIN_D3D11_GET_BACKBUFFER;
	}

	// Create render target view for backbuffer
	if (m_device->CreateRenderTargetView(backbuffer, NULL, &m_render_target_view) != S_OK)
	{
		JFATAL(ErrorCode::ERR_RENDER_TARGET_VIEW_D3D11_CREATION, "Failed to create RenderTargetView.");
		return ErrorCode::ERR_RENDER_TARGET_VIEW_D3D11_CREATION;
	}

	// ---------------------------------------------------
	// Depth/Stencil View
	// ---------------------------------------------------

	// Describe Depth/Stencil Buffer Desc
	D3D11_TEXTURE2D_DESC depth_stencil_desc = { 0 };
	depth_stencil_desc.Width = static_cast<u32>(window.width);           // Depth/Stencil buffer width
	depth_stencil_desc.Height = static_cast<u32>(window.height);         // Depth/Stencil buffer height
	depth_stencil_desc.MipLevels = m_mip_levels;                         // Number of mipmap levels
	depth_stencil_desc.ArraySize = 1;                                    // Number of textures in array
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;           // Color format

	// Use 4x MSAA? It must match swap chain MSAA values.
	if (m_4xmsaa_enabled)
	{
		depth_stencil_desc.SampleDesc.Count = 4;                         // Samples per pixel (antialiasing)
		depth_stencil_desc.SampleDesc.Quality = m_4xmsaa_quality - 1;    // Level of image quality
	}
	// No MSAA
	else
	{
		depth_stencil_desc.SampleDesc.Count = 1;
		depth_stencil_desc.SampleDesc.Quality = 0;
	}

	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;                      // Default - GPU will both read and write to the resource
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;             // Where resource will be bound to the pipeline
	depth_stencil_desc.CPUAccessFlags = 0;                               // CPU will not read not write to the Depth/Stencil buffer
	depth_stencil_desc.MiscFlags = 0;                                    // Optional flags

	// Create Depth/Stencil Buffer
	if (m_device->CreateTexture2D(&depth_stencil_desc, 0, &m_depth_stencil_buffer) != S_OK)
	{
		JFATAL(ErrorCode::ERR_DEPTHSTENCIL_D3D11_BUFFER, "Failed to create DepthStencil buffer (Texture2D).");
		return ErrorCode::ERR_DEPTHSTENCIL_D3D11_BUFFER;
	}

	// Create Depth/Stencil View
	if (m_device->CreateDepthStencilView(m_depth_stencil_buffer, 0, &m_depth_stencil_view) != S_OK)
	{
		JFATAL(ErrorCode::ERR_DEPTHSTENCIL_VIEW_D3D11_CREATION, "Failed to create DepthStencilView.");
		return ErrorCode::ERR_DEPTHSTENCIL_VIEW_D3D11_CREATION;
	}

	// Bind render target and depth stencil to the Output Merger stage
	m_device_context->OMSetRenderTargets(1, &m_render_target_view, m_depth_stencil_view);

	// ---------------------------------------------------
	// Viewport
	// ---------------------------------------------------

	// Describe Viewport
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<f32>(window.width);
	m_viewport.Height = static_cast<f32>(window.height);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	// Set Viewport
	m_device_context->RSSetViewports(1, &m_viewport);

	// ---------------------------------------------
	// Blend State
	// ---------------------------------------------

	// Describe blend state
	D3D11_BLEND_DESC blend_desc = { 0 };
	blend_desc.AlphaToCoverageEnable = false;                                // Highlight the silhouette of sprites
	blend_desc.IndependentBlendEnable = false;                               // Use the same mix for all render targets
	blend_desc.RenderTarget[0].BlendEnable = true;                           // Enable blending
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;             // Source mixing factor
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;        // Target of RGB mixing is inverted alpha
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;                 // Addition operation in color mixing
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;        // Alpha blend source is the alpha of the pixel shader
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;   // Fate of Alpha mixture is inverted alpha
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;            // Addition operation in color mixing
	blend_desc.RenderTarget[0].RenderTargetWriteMask = 0x0F;                 // Components of each pixel that can be overwritten

	// Create blend state
	if (m_device->CreateBlendState(&blend_desc, &m_blend_state) != S_OK)
	{
		JFATAL(ErrorCode::ERR_BLENDSTATE_D3D11_CREATION, "Failed to create BlendState.");
		return ErrorCode::ERR_BLENDSTATE_D3D11_CREATION;
	}

	// Bind blend state to the Output Merger stage
	m_device_context->OMSetBlendState(m_blend_state, nullptr, 0xffffffff);

	// ---------------------------------------------------
	// Rasterizer
	// ---------------------------------------------------

	// Describe rasterizer
	D3D11_RASTERIZER_DESC rasterizer_desc = { };
	ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.DepthClipEnable = true;

	// Create Solid rasterizer state
	if (m_device->CreateRasterizerState(&rasterizer_desc, &m_rasterizer_state_solid) != S_OK)
	{
		JFATAL(ErrorCode::ERR_RASTERIZER_D3D11_CREATION, "Failed to create RasterizerState.");
		return ErrorCode::ERR_RASTERIZER_D3D11_CREATION;
	}

	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;

	// Create Wireframe rasterizer state
	if (m_device->CreateRasterizerState(&rasterizer_desc, &m_rasterizer_state_wireframe) != S_OK)
	{
		JFATAL(ErrorCode::ERR_RASTERIZER_D3D11_CREATION, "Failed to create RasterizerState.");
		return ErrorCode::ERR_RASTERIZER_D3D11_CREATION;
	}

	// Set Solid rasterizer state as default
	m_device_context->RSSetState(m_rasterizer_state_solid);

	// ---------------------------------------------------
	//	Release Resources
	// ---------------------------------------------------

	backbuffer->Release();

	return ErrorCode::OK;
}

void joj::D3D11Renderer::clear(f32 r, f32 g, f32 b, f32 a)
{
	// Background color of the backbuffer = window background color
	f32 bgcolor[4]{ r, g, b, a };
	m_device_context->ClearRenderTargetView(m_render_target_view, bgcolor);
	m_device_context->ClearDepthStencilView(m_depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void joj::D3D11Renderer::render()
{

}

void joj::D3D11Renderer::swap_buffers()
{
	if (m_swapchain->Present(m_vsync, NULL) != S_OK)
	{
		JERROR(ErrorCode::ERR_RENDERER_D3D11_SWAPCHAIN_PRESENT, "Failed to present SwapChain.");
		return;
	}

	m_device_context->OMSetRenderTargets(1, &m_render_target_view, m_depth_stencil_view);
}

void joj::D3D11Renderer::set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY topology) const
{
	m_device_context->IASetPrimitiveTopology(topology);
}

void joj::D3D11Renderer::set_rasterizer_fill_mode(RasterizerFillMode mode)
{
	switch (mode)
	{
	case RasterizerFillMode::Solid:
		m_device_context->RSSetState(m_rasterizer_state_solid);
		break;
	case RasterizerFillMode::Wireframe:
		m_device_context->RSSetState(m_rasterizer_state_wireframe);
		break;
	default:
		m_device_context->RSSetState(m_rasterizer_state_solid);
		break;
	}
}

#endif // JPLATFORM_WINDOWS