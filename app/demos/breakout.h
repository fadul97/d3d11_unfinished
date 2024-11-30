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
#include "joj/systems/ecs/component_map.h"
#include "joj/systems/ecs/components.h"
#include "joj/systems/ecs/movement_system.h"

struct Block
{
	joj::Object2D object;
	b8 active = true;
	joj::JFloat4 color;
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

	joj::Entity player;
	joj::Entity ball;
	joj::Entity block;
	joj::ComponentMap<joj::Position2D> positions;
	joj::ComponentMap<joj::Velocity2D> velocities;
	joj::Position2D player_pos{ 400.0f, 50.0f };
	joj::Size2D player_size{ 125.0f, 25.0f };
	joj::Position2D ball_pos{ 0.0f, 0.0f };
	joj::Size2D ball_size{ 20.0f, 20.0f };
	joj::JFloat4 player_color{ DirectX::Colors::AliceBlue };

	joj::JFloat2 player_velocity{ 400.0f, 0.0f };
	joj::JFloat2 ball_velocity{ 400.0f, 400.0f };

	b8 started = false;
	joj::JFloat4 ball_color{ DirectX::Colors::PaleVioletRed };

	std::vector<Block> blocks;
	i32 blocks_active = 0;

	joj::D3D11VertexBuffer vb;
	joj::D3D11IndexBuffer ib;
	joj::D3D11ConstantBuffer cb;

	ID3D11ShaderResourceView* m_brick = nullptr;
	ID3D11ShaderResourceView* m_checkboard = nullptr;
	ID3D11ShaderResourceView* m_ball_tex = nullptr;
	ID3D11SamplerState* m_sampler_state = nullptr;
};

#endif // BREAKOUT_H