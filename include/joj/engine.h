#ifndef JOJ_ENGINE_H
#define JOJ_ENGINE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include "platform/win32/window_win32.h"
#include "platform/win32/input_win32.h"
#include "platform/win32/timer_win32.h"
#include "renderer/d3d11/renderer_d3d11.h"
#include "application/app.h"

namespace joj
{
    class JAPI Engine
    {
    public:
        Engine();
        ~Engine();

        // Static members for now
        static Win32Window* s_window;
        static Win32Input* s_input;
        static Win32Timer* s_timer;
        static D3D11Renderer* s_renderer;
        static f32 s_frametime;

        static App* s_app;
        ErrorCode start();
        i32 run(App* app);

        static void close();

        static void pause();	// Pause engine
        static void resume();	// Resume engine

        // Handle Windows events
        static LRESULT CALLBACK EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    private:
        static b8 s_paused;
        static b8 s_running;
        f32 get_frametime();
    };

    inline void Engine::pause()
	{ s_paused = true; s_timer->stop(); }

	inline void Engine::resume()
	{ s_paused = false; s_timer->start(); }

    inline void Engine::close()
	{ s_running = false; }
}

#endif // JOJ_ENGINE_H