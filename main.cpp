#include <iostream>
#include "joj/defines.h"
#include "joj/logger.h"

int main()
{
    i32 a = 5;
    std::cout << "a = " << a << std::endl;

    std::cout << "Hello, Joj!" << std::endl;

    JINFO("Hello, Joj!");
    JDEBUG("Hello, Joj!");
    JWARN("Hello, Joj!");
    JERROR(joj::ErrorCode::FAILED, "Hello, Joj!");
    JFATAL(joj::ErrorCode::FAILED, "Hello, Joj!");

    return 0;
}