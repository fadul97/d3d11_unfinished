#ifndef JOJ_WIN32_TIMER_H
#define JOJ_WIN32_TIMER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <windows.h>
#include <mmsystem.h> // timeBeginPeriod and timeEndPeriod functions
// #pragma comment(lib, "winmm.lib") // Ensure linking against winmm.lib

namespace joj
{
    class JAPI Win32Timer
    {
    public:
        Win32Timer();
        ~Win32Timer();

        void start();
        void stop();
        f64 reset();
        f64 elapsed();

        b8 was_elapsed(f64 secs);
        b8 is_cumulative_elapsed_more_than(f64 secs);
        f64 total_elapsed() const;

        long long stamp();
        f64 elapsed_since(long long stamp);
        b8 was_elapsed_since(long long stamp, f64 secs);

        void begin_period();
        void end_period();

    private:
        LARGE_INTEGER m_counter_start;
        LARGE_INTEGER m_end;
        static LARGE_INTEGER m_freq;
        b8 m_stopped;
        f64 m_cumulative_elapsed;
    };

    inline void Win32Timer::begin_period()
    { timeBeginPeriod(1); }

    inline void Win32Timer::end_period()
    { timeEndPeriod(1); }

    inline b8 Win32Timer::was_elapsed(f64 secs)
    { return (elapsed() >= secs ? true : false); }

    inline b8 Win32Timer::was_elapsed_since(long long stamp, f64 secs)
    { return (elapsed_since(stamp) >= secs ? true : false); }

    inline f64 Win32Timer::total_elapsed() const
    { return m_cumulative_elapsed; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_WIN32_TIMER_H