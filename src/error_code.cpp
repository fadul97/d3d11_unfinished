#include "error_code.h"

int joj::err_to_int(const ErrorCode err)
{
    switch (err)
    {
    case ErrorCode::OK:
        return 0;

    case ErrorCode::FAILED:
        return -1;

    default:
        return -1;
    }
}

const char* joj::err_to_str(const ErrorCode err)
{
    switch (err)
    {
    case ErrorCode::OK:
        return "OK";

    case ErrorCode::FAILED:
        return "FAILED";

    default:
        return "UNKNOWN";
    }
}