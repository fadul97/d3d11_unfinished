#include <iostream>
#include "joj/logger.h"
#include "joj/platform/win32/window_win32.h"
#include "joj/platform/win32/input_win32.h"
#include "joj/platform/win32/timer_win32.h"
#include "joj/renderer/d3d11/renderer_d3d11.h"
#include "joj/jmacros.h"
#include "box_demo.h"
#include <sstream>

/* TODO: Add comments for new files and refactor Demo App. */

f32 get_frametime(HWND handle, joj::Win32Timer& timer);
f32 frametime = 0.0f;
f32 dt = 0.0f;

int main()
{
    joj::Win32Window window;
    JOJ_LOG_IF_FAIL(window.create(800, 600, "Joj Window", joj::WindowMode::Windowed));

    joj::Win32Input input;
    input.set_window(window.get_window_data().handle);

    u16 width = 0, height = 0;
    window.get_window_size(width, height);
    JDEBUG("Window size: %dx%d", width, height);

    window.get_client_size(width, height);
    JDEBUG("Window client size: %dx%d", width, height);

    joj::D3D11Renderer renderer;
    JOJ_LOG_IF_FAIL(renderer.init(window.get_window_data()));

    BoxDemo app;
    app.init();
    app.build_geometry_buffers(renderer);
    app.build_shaders(renderer);
    app.build_vertex_layout(renderer);
    app.build_constant_buffer(renderer);

    joj::Win32Timer timer;
    timer.begin_period();
    timer.start();

    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        frametime = get_frametime(window.get_window_data().handle, timer);

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (input.is_key_pressed('A'))
            JDEBUG("A pressed");

        if (input.is_key_down(joj::KEY_SPACE))
            JDEBUG("SPACE down");

        app.update(frametime);

        renderer.clear();

        renderer.get_device_context()->IASetInputLayout(app.m_input_layout);
        renderer.get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        renderer.get_device_context()->IASetVertexBuffers(0, 1, &app.m_box_vb, &stride, &offset);
        renderer.get_device_context()->IASetIndexBuffer(app.m_box_ib, DXGI_FORMAT_R32_UINT, 0);

        renderer.get_device_context()->VSSetShader(
            // Pointer to a vertex shader
            app.m_shader.get_vertex_shader(),
            // A pointer to an array of class-instance interfaces
            nullptr,
            // The number of class-instance interfaces in the array
            0u);

        renderer.get_device_context()->PSSetShader(
            // Pointer to a vertex shader
            app.m_shader.get_pixel_shader(),
            // A pointer to an array of class-instance interfaces
            nullptr,
            // The number of class-instance interfaces in the array
            0u);

        // Set constants
        DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&app.mWorld);
        DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&app.mView);
        DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&app.mProj);
        DirectX::XMMATRIX worldViewProj = world * view * proj;

        ObjectConstants cbPerObject;
        XMStoreFloat4x4(&cbPerObject.World, XMMatrixTranspose(worldViewProj));

        // Provides access to subresource data
        D3D11_MAPPED_SUBRESOURCE msr;
        // Gets a pointer to the data contained in a subresource, and denies the GPU access to that subresource
        if (renderer.get_device_context()->Map(
            // A pointer to a ID3D11Resource interface
            app.m_obj_cb,
            // Index number of the subresource
            0u,
            // A D3D11_MAP-typed value that specifies the CPU's read and write permissions for a resource
            D3D11_MAP_WRITE_DISCARD,
            // Flag that specifies what the CPU does when the GPU is busy
            0u,
            // output pointer
            &msr
        ) != S_OK)
        {
            JERROR(joj::ErrorCode::FAILED, "Failed to map Constant Buffer.");
        }

        memcpy(msr.pData, &cbPerObject, sizeof(cbPerObject));

        // Invalidate the pointer to a resource and reenable the GPU's access to that resource
        renderer.get_device_context()->Unmap(app.m_obj_cb, 0u);

        renderer.get_device_context()->VSSetConstantBuffers(0, 1, &app.m_obj_cb);

        renderer.get_device_context()->DrawIndexed(36, 0, 0);
        renderer.swap_buffers();
    }

    timer.end_period();

    app.shutdown();

    JDEBUG("Hello, Joj!");

    return 0;
}

f32 get_frametime(HWND handle, joj::Win32Timer& timer)
{
#ifdef _DEBUG
    static f32 total_time = 0.0f;	// Total time elapsed
    static u32  frame_count = 0;	// Elapsed frame counter
#endif

    // Current frame time
    frametime = timer.reset();

#ifdef _DEBUG
    // Accumulated frametime
    total_time += frametime;

    // Increment frame counter
    frame_count++;

    // Updates FPS indicator in the window every 1000ms (1 second)
    if (total_time >= 1.0f)
    {
        std::stringstream text;		// Text flow for messages
        text << std::fixed;			// Always show the fractional part
        text.precision(3);			// three numbers after comma

        text << "Joj Engine" << "    "
            << "FPS: " << frame_count << "    "
            << "Frametime: " << frametime * 1000 << " (ms)";

        SetWindowText(handle, text.str().c_str());

        frame_count = 0;
        total_time -= 1.0f;
    }
#endif

    return frametime;
}