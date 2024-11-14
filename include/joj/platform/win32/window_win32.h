#ifndef JOJ_WIN32_WINDOW_H
#define JOJ_WIN32_WINDOW_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#include <iostream>

JINLINE void win32_print()
{
    std::cout << "Print from WIN32" << std::endl;
}

#endif // JOJ_WIN32_WINDOW_H