#ifndef BOX_DEMO_H
#define BOX_DEMO_H

#include "joj/application/app.h"

class BoxDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

private:
};

#endif // BOX_DEMO_H