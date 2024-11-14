#include <iostream>
#include "joj/logger.h"
#include "joj/platform/win32/window_win32.h"
#include "joj/platform/win32/input_win32.h"

int main()
{
    i32 a = 5;
    std::cout << "a = " << a << std::endl;

    joj::Win32Window window;
    window.create(800, 600, "Joj Window", joj::WindowMode::Windowed);

    joj::Win32Input input;
    input.set_window(window.get_window_data().handle);

    u16 width = 0, height = 0;
    window.get_window_size(width, height);
    JDEBUG("Window size: %dx%d", width, height);

    window.get_client_size(width, height);
    JDEBUG("Window client size: %dx%d", width, height);

    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (input.is_key_pressed('A'))
            JDEBUG("A pressed");

        if (input.is_key_down(joj::KEY_SPACE))
            JDEBUG("SPACE down");
    }

    win32_print();

    std::cout << "Hello, Joj!" << std::endl;

    JINFO("Hello, Joj!");
    JDEBUG("Hello, Joj!");
    JWARN("Hello, Joj!");
    JERROR(joj::ErrorCode::FAILED, "Hello, Joj!");
    JFATAL(joj::ErrorCode::FAILED, "Hello, Joj!");

    return 0;
}