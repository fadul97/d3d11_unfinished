#include "platform/win32/input_win32.h"

#if JPLATFORM_WINDOWS

#include "platform/win32/window_win32.h"
#include <windowsx.h>

joj::InputData joj::Win32Input::m_data{};

joj::Win32Input::Win32Input()
{
}

joj::Win32Input::~Win32Input()
{
}

void joj::Win32Input::set_window(HWND hwnd)
{
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)InputProc);
}

b8 joj::Win32Input::is_key_pressed(const u32 key)
{
    if (m_data.ctrl.keys[key])
    {
        if (is_key_down(key))
        {
            m_data.ctrl.keys[key] = false;
            return true;
        }
    }
    else if (is_key_up(key))
    {
        m_data.ctrl.keys[key] = true;
    }

    return false;
}

LRESULT CALLBACK joj::Win32Input::InputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        // Key pressed
    case WM_KEYDOWN:
        m_data.keyboard.keys[wParam] = true;
        return 0;

        // Key released
    case WM_KEYUP:
        m_data.keyboard.keys[wParam] = false;
        return 0;

        // Mouse movement
    case WM_MOUSEMOVE:
        m_data.mouse.x = GET_X_LPARAM(lParam);
        m_data.mouse.y = GET_Y_LPARAM(lParam);
        return 0;

        // Mouse wheel movement
    case WM_MOUSEWHEEL:
        m_data.mouse.wheel = GET_WHEEL_DELTA_WPARAM(wParam);
        return 0;

        // Left mouse button pressed
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        m_data.keyboard.keys[VK_LBUTTON] = true;
        m_data.mouse.buttons[BUTTON_LEFT] = true;
        return 0;

        // Middle mouse button pressed
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
        m_data.keyboard.keys[VK_MBUTTON] = true;
        m_data.mouse.buttons[BUTTON_MIDDLE] = true;
        return 0;

        // Right mouse button pressed
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
        m_data.keyboard.keys[VK_RBUTTON] = true;
        m_data.mouse.buttons[BUTTON_RIGHT] = true;
        return 0;

        // Left mouse button released
    case WM_LBUTTONUP:
        m_data.keyboard.keys[VK_LBUTTON] = false;
        m_data.mouse.buttons[BUTTON_LEFT] = false;
        return 0;

        // Middle mouse button released
    case WM_MBUTTONUP:
        m_data.keyboard.keys[VK_MBUTTON] = false;
        m_data.mouse.buttons[BUTTON_MIDDLE] = false;
        return 0;

        // Right mouse button released
    case WM_RBUTTONUP:
        m_data.keyboard.keys[VK_RBUTTON] = false;
        m_data.mouse.buttons[BUTTON_RIGHT] = false;
        return 0;

    default:
        return CallWindowProc(Win32Window::WinProc, hWnd, msg, wParam, lParam);
    }
}

#endif // JPLATFORM_WINDOWS