/**
 * @file error_code.h
 * @author Leonardo Fadul ( TODO: todo@domain.com)
 * @brief This file contains the error codes used throughout the engine,
 * and some helper functions for debug purposes.
 * @version 0.1
 * @date 2024-11-13
 * 
 * @copyright TODO: Copyright (c) 2024
 * 
 * REFERENCES: Travis Vroman (https://travisvroman.com/) - Kohi Game Engine (https://github.com/travisvroman/kohi)
 * 
 */

#ifndef JOJ_ERROR_CODE_H
#define JOJ_ERROR_CODE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

namespace joj
{
    /** @brief Represents the error codes for the engine. */
    enum class ErrorCode
    {
        /** @brief OK code, should be used when the operation was successful. */
        OK,

        /** @brief Generic error code, should be used when the operation failed. */
        FAILED,
    };

    /** @brief Get the index of the ErrorCode. */
    i32 err_to_int(ErrorCode err);

    /** @brief Get the name of the ErrorCode. */
    const char* err_to_str(ErrorCode err);
}

#endif // JOJ_ERROR_CODE_H