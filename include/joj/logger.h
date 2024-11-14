#ifndef JOJ_LOGGER_H
#define JOJ_LOGGER_H

#include "error_code.h"
#include <vadefs.h>

#define JRELEASE 0

#if JRELEASE == 1
#define LOG_WARN_ENABLED 0
#define LOG_DEBUG_ENABLED 0
#define LOG_INFO_ENABLED 0
#else
#define LOG_WARN_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_INFO_ENABLED 1
#endif

namespace joj
{
    enum class LogLevel : i32
    {
        LOG_LEVEL_FATAL = 0,
        LOG_LEVEL_ERROR = 1,
        LOG_LEVEL_WARN = 2,
        LOG_LEVEL_DEBUG = 3,
        LOG_LEVEL_INFO = 4,
    };

    namespace Logger
    {
        void write_log(LogLevel level, joj::ErrorCode err, const char* file, i32 line, const char* message, va_list args);
        void log(LogLevel level, joj::ErrorCode err, const char* file, i32 line, const char* message, ...);
    }
}

#if LOG_INFO_ENABLED == 1
#define JINFO(message, ...) joj::Logger::log(joj::LogLevel::LOG_LEVEL_INFO, joj::ErrorCode::OK, __FILE__, __LINE__, message, ##__VA_ARGS__);
#else
#define JINFO(message, ...);
#endif

#if LOG_DEBUG_ENABLED == 1
#define JDEBUG(message, ...) joj::Logger::log(joj::LogLevel::LOG_LEVEL_DEBUG, joj::ErrorCode::OK, __FILE__, __LINE__, message, ##__VA_ARGS__);
#else
#define JDEBUG(message, ...);
#endif

#if LOG_WARN_ENABLED == 1
#define JWARN(message, ...) joj::Logger::log(joj::LogLevel::LOG_LEVEL_WARN, joj::ErrorCode::OK,  __FILE__, __LINE__, message, ##__VA_ARGS__);
#else
#define JWARN(message, ...);
#endif

#ifndef JERROR
#define JERROR(error, message, ...) joj::Logger::log(joj::LogLevel::LOG_LEVEL_ERROR, error, __FILE__, __LINE__, message, ##__VA_ARGS__);
#endif

#ifndef JFATAL
#define JFATAL(error, message, ...) joj::Logger::log(joj::LogLevel::LOG_LEVEL_FATAL, error, __FILE__, __LINE__, message, ##__VA_ARGS__);
#endif

#endif // JOJ_LOGGER_H