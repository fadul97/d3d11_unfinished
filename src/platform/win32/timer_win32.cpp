#include "platform/win32/timer_win32.h"

#if JPLATFORM_WINDOWS

LARGE_INTEGER joj::Win32Timer::m_freq = {};

joj::Win32Timer::Win32Timer()
{
    // Get frequency from high-resolution counter
    if (!m_freq.QuadPart)
        QueryPerformanceFrequency(&m_freq);

    // Reset the start and end values of the counter
    ZeroMemory(&m_counter_start, sizeof(m_counter_start));
    ZeroMemory(&m_end, sizeof(m_end));

    // Timer running
    m_stopped = false;
    m_cumulative_elapsed = 0.0f;
}

joj::Win32Timer::~Win32Timer()
{
}

void joj::Win32Timer::start()
{
    if (m_stopped)
    {
        // Resume count time
        //
        //      <--- elapsed ---->
        // ----|------------------|------------> time
        //    start               end
        //

        // Time elapsed before stopping
        const i64 elapsed = m_end.QuadPart - m_counter_start.QuadPart;

        // Takes into account time already elapsed before the stop
        QueryPerformanceCounter(&m_counter_start);
        m_counter_start.QuadPart -= elapsed;

        // Resume normal counting
        m_stopped = false;
    }
    else
    {
        // Start counting time
        QueryPerformanceCounter(&m_counter_start);
    }
}

void joj::Win32Timer::stop()
{
    if (!m_stopped)
    {
        // Mark the stopping point of time
        QueryPerformanceCounter(&m_end);
        m_stopped = true;
    }
}

f64 joj::Win32Timer::reset()
{
    // To keep track of time elapsed
    i64 elapsed;

    if (m_stopped)
    {
        // Get time elapsed before stopping
        elapsed = m_end.QuadPart - m_counter_start.QuadPart;

        // Reset time count
        QueryPerformanceCounter(&m_counter_start);

        // Count reactivated
        m_stopped = false;
    }
    else
    {
        // End time counting
        QueryPerformanceCounter(&m_end);

        // Calculate elapsed time (in cycles)
        elapsed = m_end.QuadPart - m_counter_start.QuadPart;

        // Reset counter
        m_counter_start = m_end;
    }

    // Add to cumulative elapsed time
    m_cumulative_elapsed += elapsed / static_cast<f64>(m_freq.QuadPart);

    // Convert time to seconds
    return elapsed / static_cast<f64>(m_freq.QuadPart);
}

f64 joj::Win32Timer::elapsed()
{
    // To keep track of time elapsed
    i64 elapsed;

    if (m_stopped)
    {
        // Get time elapsed until the stop
        elapsed = m_end.QuadPart - m_counter_start.QuadPart;

    }
    else
    {
        // End time counting
        QueryPerformanceCounter(&m_end);

        // Calculate elapsed time (in cycles)
        elapsed = m_end.QuadPart - m_counter_start.QuadPart;
    }

    // Convert time to seconds
    return elapsed / static_cast<f64>(m_freq.QuadPart);
}

long long joj::Win32Timer::stamp()
{
    QueryPerformanceCounter(&m_end);
    return m_end.QuadPart;
}

f64 joj::Win32Timer::elapsed_since(long long stamp)
{
    // To keep track of time elapsed
    i64 elapsed;

    if (m_stopped)
    {
        // Get time elapsed until the stop
        elapsed = m_end.QuadPart - stamp;

    }
    else
    {
        // End time counting
        QueryPerformanceCounter(&m_end);

        // Calculate elapsed time (in cycles)
        elapsed = m_end.QuadPart - stamp;
    }

    // Convert time to seconds
    return elapsed / f64(m_freq.QuadPart);
}

b8 joj::Win32Timer::is_cumulative_elapsed_more_than(f64 secs)
{
    return (m_cumulative_elapsed >= secs ? true : false);
}

#endif // JPLATFORM_WINDOWS