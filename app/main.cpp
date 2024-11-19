#include <iostream>
#include "joj/logger.h"
#include "joj/platform/win32/window_win32.h"
#include "joj/platform/win32/input_win32.h"
#include "joj/platform/win32/timer_win32.h"
#include "joj/renderer/d3d11/renderer_d3d11.h"
#include "joj/jmacros.h"
#include "demos/mirror_demo.h"
#include <sstream>
#include <math/jmath.h>
#include "joj/engine.h"

/* TODO: Add comments for new files and refactor Demo App. */

int main()
{
    joj::Engine engine;
    JOJ_RETURN_INT_IF_FAIL(engine.start());

    MirrorDemo app;
    return engine.run(&app);
}