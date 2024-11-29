#ifndef TEST2D_H
#define TEST2D_H

#include "joj/application/app.h"
#include "joj/resources/geometry/quad.h"
#include "joj/renderer/d3d11/shader_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include "joj/renderer/d3d11/index_buffer_d3d11.h"
#include "joj/renderer/d3d11/constant_buffer_d3d11.h"
#include <DirectXColors.h>

class Test2D : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

private:
	joj::D3D11Shader m_shader;
	ID3D11InputLayout* m_input_layout = nullptr;

	joj::Quad p1{ 1.0f, 1.0f };
	joj::JFloat2 p1_pos{ 30.0f, 300.0f };
	joj::JFloat2 p1_size{ 20.0f, 100.0f };

	joj::Quad p2{ 1.0f, 1.0f };
	joj::JFloat2 p2_pos{ 770.0f, 300.0f };
	joj::JFloat2 p2_size{ 20.0f, 100.0f };

	joj::Quad ball{ 1.0f, 1.0f };
	joj::JFloat2 ball_pos{ 400.0f, 300.0f };
	joj::JFloat2 ball_size{ 20.0f, 20.0f };

	joj::D3D11VertexBuffer vb;
	joj::D3D11IndexBuffer ib;
	joj::D3D11ConstantBuffer cb;
};

#endif // TEST2D_H