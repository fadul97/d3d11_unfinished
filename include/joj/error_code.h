#ifndef JOJ_ERROR_CODE_H
#define JOJ_ERROR_CODE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

namespace joj
{
    enum class ErrorCode
    {
        OK,
        FAILED,
    };

    i32 err_to_int(ErrorCode err);
    const char* err_to_str(ErrorCode err);
}

#endif // JOJ_ERROR_CODE_H