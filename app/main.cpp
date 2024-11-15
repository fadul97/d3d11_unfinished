#include <iostream>
#include "joj/logger.h"
#include "joj/platform/win32/window_win32.h"
#include "joj/platform/win32/input_win32.h"
#include "joj/platform/win32/timer_win32.h"
#include "joj/renderer/d3d11/renderer_d3d11.h"
#include "joj/jmacros.h"
#include "box_demo.h"
#include <sstream>

f32 get_frametime(HWND handle, joj::Win32Timer& timer);
f32 frametime = 0.0f;
f32 dt = 0.0f;

int main()
{
    BoxDemo app;
    app.init();

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

        renderer.clear();
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