#ifndef BREAKOUT_H
#define BREAKOUT_H

#include "joj/application/app.h"
#include "joj/renderer/d3d11/shader_d3d11.h"
#include "joj/renderer/d3d11/vertex_buffer_d3d11.h"
#include "joj/renderer/d3d11/index_buffer_d3d11.h"
#include "joj/renderer/d3d11/constant_buffer_d3d11.h"
#include <DirectXColors.h>
#include "joj/systems/objects2d/object2d.h"
#include <vector>

struct Block
{
	joj::Object2D object; // Representa a posição e tamanho do bloco
	b8 active = true;   // Determina se o bloco está ativo ou foi removido
	joj::JFloat4 color;   // Cor do bloco
};

class BreakoutGame : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

	void draw_blocks();

private:
	joj::D3D11Shader m_shader;
	ID3D11InputLayout* m_input_layout = nullptr;

	joj::Object2D player{ 400.0f, 50.0f, 125.0f, 25.0f };
	joj::JFloat4 player_color{ DirectX::Colors::AliceBlue };

	f32 player_velocity = 400.0f;

	b8 started = false;
	joj::Object2D ball{ 0.0f, 0.0f, 20.0f, 20.0f};
	joj::JFloat4 ball_color{ DirectX::Colors::PaleVioletRed };
	joj::JFloat2 ball_velocity{ 400.0f, 400.0f };

	std::vector<Block> blocks;
	i32 blocks_active = 0;

	joj::D3D11VertexBuffer vb;
	joj::D3D11IndexBuffer ib;
	joj::D3D11ConstantBuffer cb;
};

#endif // BREAKOUT_H