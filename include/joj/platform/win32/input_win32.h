#ifndef JOJ_WIN32_INPUT_H
#define JOJ_WIN32_INPUT_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "platform/keys.h"

namespace joj
{
    struct InputData
    {
        Keyboard keyboard;
        Keyboard ctrl;
        Mouse mouse;
    };

    class JAPI Win32Input
    {
    public:
        Win32Input();
        ~Win32Input();

        void set_window(HWND hwnd);

        b8 is_key_down(u32 key);
        b8 is_key_pressed(u32 key);
        b8 is_key_up(u32 key);

        b8 is_button_down(Buttons button) const;
        b8 is_button_up(Buttons button) const;

        i16 get_xmouse();
        i16 get_ymouse();
        i16 get_mouse_wheel();

        static LRESULT CALLBACK InputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        static InputData m_data;
    };

    inline b8 Win32Input::is_key_down(const u32 key)
    { return m_data.keyboard.keys[key]; }

    inline b8 Win32Input::is_key_up(const u32 key)
    { return !m_data.keyboard.keys[key]; }

    inline b8 Win32Input::is_button_down(Buttons button) const
    { return m_data.mouse.buttons[button]; }

    inline b8 Win32Input::is_button_up(Buttons button) const
    { return !m_data.mouse.buttons[button]; }

    inline i16 Win32Input::get_xmouse()
    { return m_data.mouse.x; }

    inline i16 Win32Input::get_ymouse()
    { return m_data.mouse.y; }

    inline i16 Win32Input::get_mouse_wheel()
    { return m_data.mouse.wheel; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_WIN32_INPUT_H