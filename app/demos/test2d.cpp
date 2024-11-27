#include "test2d.h"

#include "joj/engine.h"

void Test2D::init()
{
}

void Test2D::update(const f32 dt)
{
}

void Test2D::draw()
{
    joj::Engine::s_renderer->clear(1.0f, 0.0f, 1.0f, 1.0f);
    joj::Engine::s_renderer->swap_buffers();
}

void Test2D::shutdown()
{
}
