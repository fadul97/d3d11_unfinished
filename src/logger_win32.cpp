#include "logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <Windows.h>

void joj::Logger::write_log(const LogLevel level, const ErrorCode err, const char* file, const int line, const char* message, va_list args)
{
    const char* level_strings[5] =
    {
        "[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[DEBUG]: ", "[INFO]: "
    };

    // Get error name
    const char* err_name = err_to_str(err);

    // Colors (Windows)
    WORD color_attributes[5] =
    {
        FOREGROUND_RED | FOREGROUND_INTENSITY,  // Bold red
        FOREGROUND_RED,                         // Red
        FOREGROUND_RED | FOREGROUND_GREEN,      // Orange
        FOREGROUND_BLUE,                        // Blue
        FOREGROUND_GREEN                        // Green
    };

    // Get the console handle
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Cast ErrorCode to i32
    i32 level_index = static_cast<i32>(level);

    // Use regular font color
    if (console_handle == INVALID_HANDLE_VALUE)
    {
        if (err == joj::ErrorCode::OK)
            vprintf((std::string(level_strings[level_index]) + message + "\n").c_str(), args);
        else
            vprintf((std::string(level_strings[level_index]) + std::string(err_name) + message + "\n").c_str(), args);
        return;
    }

    // Prepare the log message with file and line information
    char full_message[1024];
    snprintf(full_message, sizeof(full_message), "%s%s - %s\nFile: %s\nLine: %d", level_strings[level_index], err_name, message, file, line);

    // FATAL log
    if (level == LogLevel::LOG_LEVEL_FATAL)
    {
        SetConsoleTextAttribute(console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_RED);
        vprintf((std::string(full_message) + "\n").c_str(), args);
        SetConsoleTextAttribute(console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset to default
        return;
    }
    // ERROR log
    if (level == LogLevel::LOG_LEVEL_ERROR)
    {
        SetConsoleTextAttribute(console_handle, color_attributes[level_index]);
        vprintf((std::string(full_message) + "\n").c_str(), args);
        SetConsoleTextAttribute(console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset to default
        return;
    }
    // WARN log
    if (level == LogLevel::LOG_LEVEL_WARN)
    {
        SetConsoleTextAttribute(console_handle, color_attributes[level_index]);
        vprintf((std::string(full_message) + "\n").c_str(), args);
        SetConsoleTextAttribute(console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset to default
        return;
    }

    SetConsoleTextAttribute(console_handle, color_attributes[level_index]);
    vprintf((std::string(level_strings[level_index]) + message + "\n").c_str(), args);
    SetConsoleTextAttribute(console_handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); // Reset to default
}

void joj::Logger::log(const LogLevel level, const ErrorCode err, const char* file, const int line, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    write_log(level, err, file, line, message, args);
    va_end(args);
}