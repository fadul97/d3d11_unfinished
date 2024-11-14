#include "platform/win32/window_win32.h"

#if JPLATFORM_WINDOWS

#include "logger.h"
#include <windowsx.h>

#include <iostream>
void win32_print()
{
    std::cout << "Print from WIN32 src folder!" << std::endl;
}

joj::Win32Window::Win32Window()
{
    m_data.handle = nullptr;
    m_data.hdc = nullptr;
    m_data.window_mode = WindowMode::Windowed;
    m_data.width = static_cast<u16>(GetSystemMetrics(SM_CXSCREEN));
    m_data.height = static_cast<u16>(GetSystemMetrics(SM_CYSCREEN));

    m_color = RGB(60, 60, 60);
    m_style = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE;
    m_icon = LoadIcon(nullptr, IDI_APPLICATION);
    m_cursor = LoadCursor(nullptr, IDC_ARROW);
    m_window_rect = { 0, 0, m_data.width, m_data.height };
    m_client_rect = { 0, 0, 0, 0 };
}

joj::Win32Window::~Win32Window()
{
    if (m_data.hdc != nullptr)
        ReleaseDC(m_data.handle, m_data.hdc);

    if (m_data.handle != nullptr)
        DestroyWindow(m_data.handle);
}

joj::ErrorCode joj::Win32Window::create(const u16 width, const u16 height, const char* title, const WindowMode mode)
{
    const char* joj_wnd_class_name = "JOJ_WINDOW_CLASS";

    HINSTANCE app_id = GetModuleHandle(nullptr);
    if (!app_id)
    {
        JFATAL(ErrorCode::ERR_WINDOW_HANDLE, "Failed to get module handle.");
        return ErrorCode::ERR_WINDOW_HANDLE;
    }

    WNDCLASSEX wnd_class;

    if (!GetClassInfoExA(app_id, joj_wnd_class_name, &wnd_class))
    {
        wnd_class.cbSize = sizeof(WNDCLASSEX);
        wnd_class.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wnd_class.lpfnWndProc = WinProc;
        wnd_class.cbClsExtra = 0;
        wnd_class.cbWndExtra = 0;
        wnd_class.hInstance = app_id;
        wnd_class.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wnd_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wnd_class.hbrBackground = CreateSolidBrush(RGB(60, 60, 60));
        wnd_class.lpszMenuName = nullptr;
        wnd_class.lpszClassName = joj_wnd_class_name;
        wnd_class.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

        // Register "JOJ_WINDOW_CLASS" class
        if (!RegisterClassEx(&wnd_class))
        {
            JERROR(ErrorCode::ERR_WINDOW_REGISTRATION, "Failed to register window class.");
            return ErrorCode::ERR_WINDOW_REGISTRATION;
        }
    }

    if (mode == WindowMode::Windowed)
    {
        i32 screen_width = GetSystemMetrics(SM_CXSCREEN);
        i32 screen_height = GetSystemMetrics(SM_CYSCREEN);

        if (width >= screen_width)
            m_data.width = screen_width;
        else
            m_data.width = width;

        if (height >= screen_height)
            m_data.height = screen_height;
        else
            m_data.height = height;
    }
    else if (mode == WindowMode::Fullscreen)
    {
        // Ignore width and height paremeters
        m_style = WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE;
        m_data.width = GetSystemMetrics(SM_CXSCREEN);
        m_data.height = GetSystemMetrics(SM_CYSCREEN);
    }
    else
    {
        m_style = WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE;
        m_data.width = width;
        m_data.height = height;
    }
    m_data.window_mode = mode;

    m_data.handle = CreateWindowEx(
        0,
        joj_wnd_class_name,
        title,
        m_style,
        0, 0,
        m_data.width, m_data.height,
        nullptr,
        nullptr,
        app_id,
        nullptr
    );

    if (!m_data.handle)
    {
        JFATAL(ErrorCode::ERR_WINDOW_HANDLE, "Failed to create window.");
        return ErrorCode::ERR_WINDOW_HANDLE;
    }

    RECT new_rect = { 0, 0, m_data.width, m_data.height };
    if (m_data.window_mode == WindowMode::Windowed || m_data.window_mode == WindowMode::Borderless)
    {
        if (!AdjustWindowRectEx(&new_rect,
            GetWindowStyle(m_data.handle),
            GetMenu(m_data.handle) != nullptr,
            GetWindowExStyle(m_data.handle)))
        {
            JERROR(ErrorCode::ERR_WINDOW_ADJUST, "Could not adjust window rect ex.");
        }

        LONG x1 = GetSystemMetrics(SM_CXSCREEN) / 2;
        LONG x2 = (new_rect.right - new_rect.left) / 2;
        LONG x3 = x1 - x2;

        LONG xpos = (GetSystemMetrics(SM_CXSCREEN) / 2) - ((new_rect.right - new_rect.left) / 2);
        LONG ypos = (GetSystemMetrics(SM_CYSCREEN) / 2) - ((new_rect.bottom - new_rect.top) / 2);

        if (!MoveWindow(
            m_data.handle,
            xpos,
            ypos,
            new_rect.right - new_rect.left,
            new_rect.bottom - new_rect.top,
            TRUE)
            )
        {
            JERROR(ErrorCode::ERR_WINDOW_MOVE, "Could not move window.");
        }
    }

    m_data.hdc = GetDC(m_data.handle);
    if (!m_data.hdc)
        JERROR(ErrorCode::ERR_WINDOW_DEVICE_CONTEXT, "Failed to get device context.");

    if (!GetWindowRect(m_data.handle, &m_window_rect))
        JERROR(ErrorCode::ERR_WINDOW_RECT, "Failed to get window rect.");

    if (!GetClientRect(m_data.handle, &m_client_rect))
        JERROR(ErrorCode::ERR_WINDOW_CLIENT_RECT, "Failed to get client rect.");

    return ErrorCode::OK;
}

void joj::Win32Window::destroy() const
{
    if (m_data.hdc != nullptr)
        ReleaseDC(m_data.handle, m_data.hdc);

    if (m_data.handle != nullptr)
        DestroyWindow(m_data.handle);
}

void joj::Win32Window::get_window_size(u16& width, u16& height)
{
    if (!GetWindowRect(m_data.handle, &m_window_rect))
    {
        JERROR(joj::ErrorCode::ERR_WINDOW_RECT, "Failed to get window rect.");
        return;
    }

    width = static_cast<u16>(m_window_rect.right - m_window_rect.left);
    height = static_cast<u16>(m_window_rect.bottom - m_window_rect.top);
}

void joj::Win32Window::get_client_size(u16& width, u16& height)
{
    if (!GetClientRect(m_data.handle, &m_client_rect))
    {
        JERROR(joj::ErrorCode::ERR_WINDOW_CLIENT_RECT, "Failed to get client rect.");
        return;
    }

    width = static_cast<u16>(m_client_rect.right - m_client_rect.left);
    height = static_cast<u16>(m_client_rect.bottom - m_client_rect.top);
}

LRESULT CALLBACK joj::Win32Window::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
    case WM_QUIT:
    case WM_CLOSE:
        JDEBUG("Running = false");
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

#endif // JPLATFORM_WINDOWS