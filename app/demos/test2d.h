#ifndef TEST2D_H
#define TEST2D_H

#include "joj/application/app.h"

class Test2D : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

public:
};

#endif // TEST2D_H