#ifndef PONG_GAME_H
#define PONG_GAME_H

#include "joj/application/app.h"
#include "joj/resources/geometry/quad.h"
#include "joj/renderer/d3d11/shader_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include "joj/renderer/d3d11/index_buffer_d3d11.h"
#include "joj/renderer/d3d11/constant_buffer_d3d11.h"
#include <DirectXColors.h>
#include "joj/systems/objects2d/object2d.h"

class PongGame : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

private:
	joj::D3D11Shader m_shader;
	ID3D11InputLayout* m_input_layout = nullptr;

	joj::Object2D p1{ 30.0f, 300.0f, 20.0f, 100.0f };
	joj::JFloat4 p1_color{ DirectX::Colors::AliceBlue };

	joj::Object2D p2{ 770.0f, 300.0f, 20.0f, 100.0f };
	joj::JFloat4 p2_color{ DirectX::Colors::Aquamarine };

	f32 player_velocity = 400.0f;

	joj::Object2D ball{ 400.0f, 300.0f, 20.0f, 20.0f };
	joj::JFloat4 ball_color{ DirectX::Colors::PaleVioletRed };
	joj::JFloat2 ball_velocity{ 400.0f, 400.0f };

	joj::D3D11VertexBuffer vb;
	joj::D3D11IndexBuffer ib;
	joj::D3D11ConstantBuffer cb;
};

#endif // PONG_GAME_H