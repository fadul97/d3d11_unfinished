#ifndef JOJ_WIN32_WINDOW_H
#define JOJ_WIN32_WINDOW_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "error_code.h"

JAPI void win32_print();

namespace joj
{
    enum class WindowMode { Borderless, Fullscreen, Windowed };

    struct WindowData
    {
        HWND handle;
        HDC hdc;
        WindowMode window_mode;
        u16 width;
        u16 height;
    };

    class JAPI Win32Window
    {
    public:
        Win32Window();
        ~Win32Window();

        ErrorCode create(u16 width, u16 height, const char* title, WindowMode mode);
        void destroy() const;

        void get_window_size(u16& width, u16& height);
        void get_client_size(u16& width, u16& height);

        WindowData& get_window_data();

        static LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        WindowData m_data;
        RECT m_window_rect;
        RECT m_client_rect;
        HICON m_icon;
        HCURSOR m_cursor;
        COLORREF m_color;
        DWORD m_style;
    };

    inline WindowData& Win32Window::get_window_data()
    { return m_data; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_WIN32_WINDOW_H