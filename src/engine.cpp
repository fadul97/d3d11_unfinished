#include "engine.h"

#include <sstream>

#include "logger.h"
#include "jmacros.h"

// Static members
joj::Win32Window* joj::Engine::s_window = nullptr;
joj::Win32Input* joj::Engine::s_input = nullptr;
joj::Win32Timer* joj::Engine::s_timer = nullptr;
joj::D3D11Renderer* joj::Engine::s_renderer = nullptr;
f32 joj::Engine::s_frametime = 0.0f;
b8 joj::Engine::s_paused = false;
b8 joj::Engine::s_running = false;
joj::App* joj::Engine::s_app = nullptr;

joj::Engine::Engine()
{
	s_window = new Win32Window();
	s_timer = new Win32Timer();
	s_renderer = new D3D11Renderer();
}

joj::Engine::~Engine()
{
	delete s_renderer;
	delete s_timer;
	delete s_input;
	delete s_window;
}

f32 joj::Engine::get_frametime()
{
#ifdef _DEBUG
	static f32 total_time = 0.0f;	// Total time elapsed
	static u32  frame_count = 0;	// Elapsed frame counter
#endif

	// Current frame time
	s_frametime = s_timer->reset();

#ifdef _DEBUG
	// Accumulated frametime
	total_time += s_frametime;

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
			<< "Frametime: " << s_frametime * 1000 << " (ms)";

		SetWindowText(s_window->get_window_data().handle, text.str().c_str());

		frame_count = 0;
		total_time -= 1.0f;
	}
#endif

	return s_frametime;
}

joj::ErrorCode joj::Engine::start()
{
	ErrorCode result = ErrorCode::OK;

	result = s_window->create(800, 600, "Joj Engine", joj::WindowMode::Windowed);
	if (result != joj::ErrorCode::OK) {
		return result;
	}

	s_input = new Win32Input();
	s_input->set_window(s_window->get_window_data().handle);

	// result = s_renderer->init(s_window->get_window_config());
	if JOJ_FAILED(s_renderer->init(s_window->get_window_data()))
	{
		JFATAL(ErrorCode::ERR_RENDERER_D3D11_INIT, "Failed to initialize Renderer.");
		return ErrorCode::ERR_RENDERER_D3D11_INIT;
	}

	// Change window procedure to EngineProc
	SetWindowLongPtr(s_window->get_window_data().handle, GWLP_WNDPROC, (LONG_PTR)EngineProc);

	// Adjust sleep resolution to 1 millisecond
	s_timer->begin_period();

	return ErrorCode::OK;
}

i32 joj::Engine::run(App* app)
{
	/*
	if (start() != ErrorCode::OK)
		return -1;
	*/

	// Start time counter
	s_timer->start();

	MSG msg = { 0 };

	s_app = app;
	s_app->init();

	s_running = true;
	do
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
				s_running = false;
		}
		else
		{
			// -----------------------------------------------
			// Pause/Resume Game
			// -----------------------------------------------
			// P key pauses engine
			if (s_input->is_key_pressed('P'))
			{
				if (s_paused)
					resume();
				else
					pause();
			}

			if (!s_paused)
			{
				// Calculate frametime
				s_frametime = get_frametime();

				// Update game
				s_app->update(s_frametime);

				// Game draw
				s_app->draw();
			}
			else
			{
				// Game paused
				s_app->on_pause();
			}
		}
	} while (s_running);

	s_app->shutdown();

	return 0;
}

LRESULT CALLBACK joj::Engine::EngineProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Window must be repainted
	if (msg == WM_PAINT)
		s_app->display();

	return CallWindowProc(joj::Win32Input::InputProc, hWnd, msg, wParam, lParam);
}