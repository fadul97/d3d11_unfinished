#include "platform/win32/window_win32.h"

#if JPLATFORM_WINDOWS

#include <iostream>

void win32_print()
{
    std::cout << "Print from WIN32 src folder!" << std::endl;
}

#endif // JPLATFORM_WINDOWS