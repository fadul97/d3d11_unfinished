#include "error_code.h"

int joj::err_to_int(const ErrorCode err)
{
    switch (err)
    {
    case ErrorCode::OK:
        return 0;

    case ErrorCode::FAILED:
        return -1;

    case ErrorCode::ERR_WINDOW_HANDLE:
        return -2;

    case ErrorCode::ERR_WINDOW_REGISTRATION:
        return -3;

    case ErrorCode::ERR_WINDOW_ADJUST:
        return -4;

    case ErrorCode::ERR_WINDOW_MOVE:
        return -5;

    case ErrorCode::ERR_WINDOW_DEVICE_CONTEXT:
        return -6;

    case ErrorCode::ERR_WINDOW_RECT:
        return -7;

    case ErrorCode::ERR_WINDOW_CLIENT_RECT:
        return -8;

    case ErrorCode::ERR_CONTEXT_D3D11_DXGI_FACTORY2_CREATION:
        return -9;

    case ErrorCode::ERR_CONTEXT_D3D11_DEVICE_CREATION:
        return -10;

    case ErrorCode::ERR_CONTEXT_D3D11_WARP_ADAPTER_CREATION:
        return -11;

    case ErrorCode::ERR_CONTEXT_D3D11_QUERY_INTERFACE_ID3D11_DEBUG:
        return -12;

    case ErrorCode::ERR_CONTEXT_D3D11_QUERY_INTERFACE_IDXGI_DEVICE:
        return -13;

    case ErrorCode::ERR_CONTEXT_D3D11_GET_PARENTOF_IDXGI_ADAPTER:
        return -14;

    case ErrorCode::ERR_CONTEXT_D3D11_GET_PARENTOF_IDXGI_FACTORY:
        return -15;

    case ErrorCode::ERR_D3D11CONTEXT_CREATION:
        return -16;

    case ErrorCode::ERR_SWAPCHAIN_D311_CREATION:
        return -17;

    case ErrorCode::ERR_SWAPCHAIN_D3D11_GET_BACKBUFFER:
        return -18;

    case ErrorCode::ERR_RENDER_TARGET_VIEW_D3D11_CREATION:
        return -19;

    case ErrorCode::ERR_DEPTHSTENCIL_D3D11_BUFFER:
        return -20;

    case ErrorCode::ERR_DEPTHSTENCIL_VIEW_D3D11_CREATION:
        return -21;

    case ErrorCode::ERR_BLENDSTATE_D3D11_CREATION:
        return -22;

    case ErrorCode::ERR_RASTERIZER_D3D11_CREATION:
        return -23;

    case ErrorCode::ERR_RENDERER_D3D11_SWAPCHAIN_PRESENT:
        return -24;

    case ErrorCode::ERR_SHADER_D3D11_VERTEX_COMPILATION:
        return -25;

    case ErrorCode::ERR_SHADER_D3D11_PIXEL_COMPILATION:
        return -26;

    case ErrorCode::ERR_RENDERER_D3D11_INIT:
        return -27;

    default:
        return -1;
    }
}

const char* joj::err_to_str(const ErrorCode err)
{
    switch (err)
    {
    case ErrorCode::OK:
        return "OK";

    case ErrorCode::FAILED:
        return "FAILED";

    case ErrorCode::ERR_WINDOW_HANDLE:
        return "ERR_WINDOW_HANDLE";

    case ErrorCode::ERR_WINDOW_REGISTRATION:
        return "ERR_WINDOW_REGISTRATION";

    case ErrorCode::ERR_WINDOW_ADJUST:
        return "ERR_WINDOW_ADJUST";

    case ErrorCode::ERR_WINDOW_MOVE:
        return "ERR_WINDOW_MOVE";

    case ErrorCode::ERR_WINDOW_DEVICE_CONTEXT:
        return "ERR_WINDOW_DEVICE_CONTEXT";

    case ErrorCode::ERR_WINDOW_RECT:
        return "ERR_WINDOW_RECT";

    case ErrorCode::ERR_WINDOW_CLIENT_RECT:
        return "ERR_WINDOW_CLIENT_RECT";

    case ErrorCode::ERR_CONTEXT_D3D11_DXGI_FACTORY2_CREATION:
        return "ERR_CONTEXT_D3D11_DXGI_FACTORY2_CREATION";

    case ErrorCode::ERR_CONTEXT_D3D11_DEVICE_CREATION:
        return "ERR_CONTEXT_D3D11_DEVICE_CREATION";

    case ErrorCode::ERR_CONTEXT_D3D11_WARP_ADAPTER_CREATION:
        return "ERR_CONTEXT_D3D11_WARP_ADAPTER_CREATION";

    case ErrorCode::ERR_CONTEXT_D3D11_QUERY_INTERFACE_ID3D11_DEBUG:
        return "ERR_CONTEXT_D3D11_QUERY_INTERFACE_ID3D11_DEBUG";

    case ErrorCode::ERR_CONTEXT_D3D11_QUERY_INTERFACE_IDXGI_DEVICE:
        return "ERR_CONTEXT_D3D11_QUERY_INTERFACE_IDXGI_DEVICE";

    case ErrorCode::ERR_CONTEXT_D3D11_GET_PARENTOF_IDXGI_ADAPTER:
        return "ERR_CONTEXT_D3D11_GET_PARENTOF_IDXGI_ADAPTER";

    case ErrorCode::ERR_CONTEXT_D3D11_GET_PARENTOF_IDXGI_FACTORY:
        return "ERR_CONTEXT_D3D11_GET_PARENTOF_IDXGI_FACTORY";

    case ErrorCode::ERR_D3D11CONTEXT_CREATION:
        return "ERR_D3D11CONTEXT_CREATION";

    case ErrorCode::ERR_SWAPCHAIN_D311_CREATION:
        return "ERR_SWAPCHAIN_D311_CREATION";

    case ErrorCode::ERR_SWAPCHAIN_D3D11_GET_BACKBUFFER:
        return "ERR_SWAPCHAIN_D3D11_GET_BACKBUFFER";

    case ErrorCode::ERR_RENDER_TARGET_VIEW_D3D11_CREATION:
        return "ERR_RENDER_TARGET_VIEW_D3D11_CREATION";

    case ErrorCode::ERR_DEPTHSTENCIL_D3D11_BUFFER:
        return "ERR_DEPTHSTENCIL_D3D11_BUFFER";

    case ErrorCode::ERR_DEPTHSTENCIL_VIEW_D3D11_CREATION:
        return "ERR_DEPTHSTENCIL_VIEW_D3D11_CREATION";

    case ErrorCode::ERR_BLENDSTATE_D3D11_CREATION:
        return "ERR_BLENDSTATE_D3D11_CREATION";

    case ErrorCode::ERR_RASTERIZER_D3D11_CREATION:
        return "ERR_RASTERIZER_D3D11_CREATION";

    case ErrorCode::ERR_RENDERER_D3D11_SWAPCHAIN_PRESENT:
        return "ERR_RENDERER_D3D11_SWAPCHAIN_PRESENT";

    case ErrorCode::ERR_SHADER_D3D11_VERTEX_COMPILATION:
        return "ERR_SHADER_D3D11_VERTEX_COMPILATION";

    case ErrorCode::ERR_SHADER_D3D11_PIXEL_COMPILATION:
        return "ERR_SHADER_D3D11_PIXEL_COMPILATION";

    case ErrorCode::ERR_RENDERER_D3D11_INIT:
        return "ERR_RENDERER_D3D11_INIT";

    default:
        return "UNKNOWN";
    }
}