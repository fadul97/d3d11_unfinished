#ifndef JOJ_MACROS_H
#define JOJ_MACROS_H

#include "error_code.h"
#include "logger.h"

#ifndef JOJ_FAILED
#define JOJ_FAILED(x) (((joj::ErrorCode)(x)) != joj::ErrorCode::OK)
#endif // JFAILED

#ifndef JOJ_LOG_IF_FAIL
#define JOJ_LOG_IF_FAIL(x)                                                                                         \
{                                                                                                                  \
    joj::ErrorCode result = (x);                                                                                   \
    if(result != joj::ErrorCode::OK) {                                                                             \
        joj::Logger::log(joj::LogLevel::LOG_LEVEL_ERROR, result, __FILE__, __LINE__, "Function: %s", __func__);    \
        /*return result;*/                                                                                         \
    }                                                                                                              \
}
#endif // JOJ_LOG_IF_FAIL

#endif // JOJ_MACROS_H