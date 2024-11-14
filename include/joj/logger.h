/**
 * @file logger.h
 * @author Leonardo Fadul ( TODO: todo@domain.com)
 * @brief This file contains the logging system for the engine.
 * @version 0.1
 * @date 2024-11-13
 * 
 * @copyright Copyright (c) 2024
 * 
 * REFERENCES: Travis Vroman (https://travisvroman.com/) - Kohi Game Engine (https://github.com/travisvroman/kohi)
 * 
 */

#ifndef JOJ_LOGGER_H
#define JOJ_LOGGER_H

#include "error_code.h"
#include <vadefs.h>

/** @brief Change to 1 when exporting. */
#define JRELEASE 0

/** @brief Disables Warn, Debug and Info in Release Mode. */
#if JRELEASE == 1
#define LOG_WARN_ENABLED 0
#define LOG_DEBUG_ENABLED 0
#define LOG_INFO_ENABLED 0
#else

/** @brief Indicates if warning level logging is enabled. */
#define LOG_WARN_ENABLED 1

/** @brief Indicates if debug level logging is enabled */
#define LOG_DEBUG_ENABLED 1

/** @brief Indicates if info level logging is enabled. */
#define LOG_INFO_ENABLED 1

#endif

namespace joj
{
    /** @brief Represents logging levels. */
    enum class LogLevel : i32
    {
        /** @brief Fatal log level, should be used to stop the application when hit. */
        LOG_LEVEL_FATAL = 0,

        /** @brief Error log level, should be used to indicate critical runtime problems that cause the application to run improperly or not at all. */
        LOG_LEVEL_ERROR = 1,

        /** @brief Warning log level, should be used to indicate non-critial problems with the application that cause it to run suboptimally. */
        LOG_LEVEL_WARN = 2,

        /** @brief Debug log level, should be used for debugging purposes. */
        LOG_LEVEL_DEBUG = 3,

        /** @brief Info log level, should be used for non-erronuous informational purposes. */
        LOG_LEVEL_INFO = 4,
    };

    namespace Logger
    {
        /**
         * @brief Outputs logging at the given level. NOTE: This should not be called directly.
         * It is recommended to use the macros defined below (JINFO, JDEBUG, JWARN, JERROR, JFATAL).
         * @param level The log level to use.
         * @param err The error code to be logged.
         * @param file The file where the log was called.
         * @param line The line where the log was called.
         * @param message The message to be logged.
         * @param args Any additional arguments (data) to be logged.
         */
        void write_log(LogLevel level, joj::ErrorCode err, const char* file, i32 line, const char* message, va_list args);

        /**
         * @brief Writes the log to the console. This is called when the macro is used.
         * 
         * @param level The log level to use.
         * @param err The error code to be logged.
         * @param file The file where the log was called.
         * @param line The line where the log was called.
         * @param message The message to be logged.
         * @param ... Any additional arguments (data) to be logged.
         */
        void log(LogLevel level, joj::ErrorCode err, const char* file, i32 line, const char* message, ...);
    }
}

#if LOG_INFO_ENABLED == 1
/**
 * @brief Logs an info-level message. Should be used for non-erronuous informational purposes.
 * @param message The message to be logged.
 * @param ... Any additional arguments (data) to be logged.
 */
#define JINFO(message, ...) joj::Logger::log(joj::LogLevel::LOG_LEVEL_INFO, joj::ErrorCode::OK, __FILE__, __LINE__, message, ##__VA_ARGS__);
#else
#define JINFO(message, ...);
#endif

#if LOG_DEBUG_ENABLED == 1
/**
 * @brief Logs a debug-level message. Should be used for debugging purposes.
 * @param message The message to be logged.
 * @param ... Any additional arguments (data) to be logged.
 */
#define JDEBUG(message, ...) joj::Logger::log(joj::LogLevel::LOG_LEVEL_DEBUG, joj::ErrorCode::OK, __FILE__, __LINE__, message, ##__VA_ARGS__);
#else
#define JDEBUG(message, ...);
#endif

#if LOG_WARN_ENABLED == 1
/**
 * @brief Logs a warning-level message. Should be used to indicate non-critial problems that cause the application to run inefficient.
 * @param message The message to be logged.
 * @param ... Any additional arguments (data) to be logged.
 */
#define JWARN(message, ...) joj::Logger::log(joj::LogLevel::LOG_LEVEL_WARN, joj::ErrorCode::OK,  __FILE__, __LINE__, message, ##__VA_ARGS__);
#else
#define JWARN(message, ...);
#endif

#ifndef JERROR
/**
 * @brief Logs an error-level message. Should be used to indicate runtime problems.
 * @param error The error code to be logged.
 * @param message The message to be logged.
 * @param ... Any additional arguments (data) to be logged.
 */
#define JERROR(error, message, ...) joj::Logger::log(joj::LogLevel::LOG_LEVEL_ERROR, error, __FILE__, __LINE__, message, ##__VA_ARGS__);
#endif

#ifndef JFATAL
/**
 * @brief Logs a fatal-level message. Should be used to stop the application when hit.
 * @param error The error code to be logged.
 * @param message The message to be logged.
 * @param ... Any additional arguments (data) to be logged.
 */
#define JFATAL(error, message, ...) joj::Logger::log(joj::LogLevel::LOG_LEVEL_FATAL, error, __FILE__, __LINE__, message, ##__VA_ARGS__);
#endif

#endif // JOJ_LOGGER_H