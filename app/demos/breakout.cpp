#include "breakout.h"

#include "joj/engine.h"
#include "joj/logger.h"
#include "joj/resources/geometry/quad.h"

struct BasicCB
{
    joj::JFloat4x4 wvp;
    joj::JFloat4 color;
};

void BreakoutGame::init()
{
    // Create Shader
    m_shader.compile_vertex_shader(L"../../../../app/shaders/color.hlsl", "VS", "vs_5_0");
    m_shader.compile_pixel_shader(L"../../../../app/shaders/color.hlsl", "PS", "ps_5_0");

    joj::Engine::s_renderer->get_device()->CreateVertexShader(
        m_shader.get_vsblob()->GetBufferPointer(),
        m_shader.get_vsblob()->GetBufferSize(),
        nullptr,
        &m_shader.get_vertex_shader());

    joj::Engine::s_renderer->get_device()->CreatePixelShader(
        m_shader.get_psblob()->GetBufferPointer(),
        m_shader.get_psblob()->GetBufferSize(),
        nullptr,
        &m_shader.get_pixel_shader());

    // Create the vertex input layout.
    std::vector<D3D11_INPUT_ELEMENT_DESC> input_desc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    if (joj::Engine::s_renderer->get_device()->CreateInputLayout(
        input_desc.data(),
        (u32)input_desc.size(),
        m_shader.get_vsblob()->GetBufferPointer(),
        m_shader.get_vsblob()->GetBufferSize(),
        &m_input_layout
    ) != S_OK)
    {
        JERROR(joj::ErrorCode::FAILED, "Failed to create Input Layout.");
    }

    joj::Quad quad(1.0f, 1.0f);

    // Setup and Create Vertex Buffer
    vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * quad.get_vertex_count(), quad.get_vertex_data());
    if (joj::Engine::s_renderer->get_device()->CreateBuffer(
        vb.get_buffer_desc(),
        vb.get_subdata(),
        &vb.get_buffer()) != S_OK)
    {
        JERROR(joj::ErrorCode::FAILED, "Failed to create vertex buffer.");
    }

    // Setup and Create Index Buffer
    ib.setup(sizeof(u32) * quad.get_index_count(), quad.get_index_data());
    if (joj::Engine::s_renderer->get_device()->CreateBuffer(
        ib.get_buffer_desc(),
        ib.get_subdata(),
        &ib.get_buffer()) != S_OK)
    {
        JERROR(joj::ErrorCode::FAILED, "Failed to create index buffer.");
    }

    // Setup and Create Constant Buffer
    cb.setup(joj::calculate_cb_byte_size(sizeof(BasicCB)), nullptr);
    if (joj::Engine::s_renderer->get_device()->CreateBuffer(
        cb.get_buffer_desc(),
        nullptr,
        &cb.get_buffer()) != S_OK)
    {
        JERROR(joj::ErrorCode::FAILED, "Failed to create constant buffer.");
    }

    // Criação dos blocos
    constexpr f32 block_width = 60.0f;
    constexpr f32 block_height = 20.0f;
    constexpr f32 spacing = 10.0f;
    constexpr i32 rows = 2;
    constexpr i32 cols = 10;

    f32 start_x = 800.0f / 2 - (cols * (block_width + spacing) - spacing) / 2;
    f32 start_y = 500.0f;

    for (i32 row = 0; row < rows; ++row)
    {
        for (i32 col = 0; col < cols; ++col)
        {
            Block block;
            block.object = joj::Object2D(
                start_x + col * (block_width + spacing),
                start_y - row * (block_height + spacing),
                block_width,
                block_height
            );
            block.color = joj::JFloat4{ DirectX::Colors::DarkOrange };
            ++blocks_active;
            blocks.push_back(block);
        }
    }
}

void BreakoutGame::update(const f32 dt)
{
    if (!started)
    {
        if (joj::Engine::s_input->is_key_down(joj::KEY_SPACE))
        {
            started = true;
            ball.move_to(player.get_xpos(), player.get_ypos() + ball.get_ysize());
            ball.translate_xpos(ball_velocity.x * dt);
            ball.translate_ypos(ball_velocity.y * dt);
        }
    }
    else
    {
        if (blocks_active > 0)
        {
            if (joj::Engine::s_input->is_key_down(joj::KEY_RIGHT))
                player.translate_xpos(player_velocity * dt);
            if (joj::Engine::s_input->is_key_down(joj::KEY_LEFT))
                player.translate_xpos(-player_velocity * dt);

            if (player.get_xpos() + player.get_xsize() / 2 >= 800.0f)
                player.move_to(800.0f - player.get_xsize() / 2, player.get_ypos());

            if (player.get_xpos() - player.get_xsize() / 2 <= 0.0f)
                player.move_to(0.0f + player.get_xsize() / 2, player.get_ypos());

            ball.translate_xpos(ball_velocity.x * dt);
            ball.translate_ypos(ball_velocity.y * dt);

            // Right side
            if (ball.get_xpos() + ball.get_xsize() / 2 >= 800.0f)
            {
                ball.move_to(800.0f - ball.get_xsize() / 2, ball.get_ypos());
                ball_velocity.x *= -1.0f;
            }

            // Left side
            if (ball.get_xpos() - ball.get_xsize() / 2 <= 0.0f)
            {
                ball.move_to(0.0f + ball.get_xsize() / 2, ball.get_ypos());
                ball_velocity.x *= -1.0f;
            }

            // Bottom side
            if (ball.get_ypos() - ball.get_ysize() / 2 <= 0.0f)
            {
                ball.move_to(ball.get_xpos(), 0.0f + ball.get_ysize() / 2);
                ball_velocity.x = 0.0f;
                ball_velocity.y = 0.0f;
                player_velocity = 0.0f;
            }

            // Top side
            if (ball.get_ypos() + ball.get_ysize() / 2 >= 600.0f)
            {
                ball.move_to(ball.get_xpos(), 600.0f - ball.get_ysize() / 2);
                ball_velocity.y *= -1.0f;
            }

            // Ball and Player collision
            if (ball.get_xpos() + ball.get_xsize() / 2 >= player.get_xpos() - player.get_xsize() / 2 &&
                ball.get_xpos() - ball.get_xsize() / 2 <= player.get_xpos() + player.get_xsize() / 2 &&
                ball.get_ypos() + ball.get_ysize() / 2 >= player.get_ypos() - player.get_ysize() / 2 &&
                ball.get_ypos() - ball.get_ysize() / 2 <= player.get_ypos() + player.get_ysize() / 2)
            {
                // Ajusta a posição da bola para fora da superfície do jogador
                ball.move_to(ball.get_xpos(), player.get_ypos() + player.get_ysize() / 2 + ball.get_ysize() / 2);

                // Inverte a direção vertical da bola
                ball_velocity.y *= -1.0f;

                // Ajusta a direção horizontal com base na posição relativa da bola e do jogador
                float player_center = player.get_xpos();
                float ball_center = ball.get_xpos();
                float offset = (ball_center - player_center) / (player.get_xsize() / 2); // Normaliza o desvio (-1.0 a 1.0)

                ball_velocity.x += offset * 0.5f; // Modifica a velocidade horizontal da bola proporcionalmente
            }

            for (Block& block : blocks)
            {
                if (block.active &&
                    ball.get_xpos() + ball.get_xsize() / 2 >= block.object.get_xpos() - block.object.get_xsize() / 2 &&
                    ball.get_xpos() - ball.get_xsize() / 2 <= block.object.get_xpos() + block.object.get_xsize() / 2 &&
                    ball.get_ypos() + ball.get_ysize() / 2 >= block.object.get_ypos() - block.object.get_ysize() / 2 &&
                    ball.get_ypos() - ball.get_ysize() / 2 <= block.object.get_ypos() + block.object.get_ysize() / 2)
                {
                    // Inverte a direção da bola
                    ball_velocity.y *= -1.0f;

                    // Marca o bloco como removido
                    block.active = false;
                    --blocks_active;
                    break; // Apenas uma colisão por frame
                }
            }
        }
    }
}

void BreakoutGame::draw()
{
    joj::Engine::s_renderer->clear();

    joj::Engine::s_renderer->get_device_context()->IASetInputLayout(m_input_layout);
    joj::Engine::s_renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(joj::GeometryVertex);
    UINT offset = 0;
    joj::Engine::s_renderer->get_device_context()->IASetVertexBuffers(0, 1, &vb.get_buffer(), &stride, &offset);
    joj::Engine::s_renderer->get_device_context()->IASetIndexBuffer(ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

    joj::Engine::s_renderer->get_device_context()->VSSetShader(m_shader.get_vertex_shader(), nullptr, 0u);
    joj::Engine::s_renderer->get_device_context()->PSSetShader(m_shader.get_pixel_shader(), nullptr, 0u);
    joj::Engine::s_renderer->get_device_context()->VSSetConstantBuffers(0, 1, &cb.get_buffer());

    auto I = joj::matrix4x4_identity();
    auto scale_mat = DirectX::XMMatrixScaling(player.get_xsize(), player.get_ysize(), 1.0f);
    auto mat = DirectX::XMMatrixTranslation(player.get_xpos(), player.get_ypos(), 0.0f);
    auto W = DirectX::XMMatrixMultiply(scale_mat, mat);
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(0, 0, -3, 1);
    DirectX::XMVECTOR target = DirectX::XMVectorZero();
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
    DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(pos, target, up);
    constexpr f32 fov_angle = 45;
    DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicOffCenterLH(
        0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f
    );
    auto wvp = W * V * P;
    BasicCB p1_cb;
    XMStoreFloat4x4(&p1_cb.wvp, XMMatrixTranspose(wvp));
    p1_cb.color = player_color;
    cb.update(joj::Engine::s_renderer->get_device_context(), p1_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(6, 0, 0);

    scale_mat = DirectX::XMMatrixScaling(ball.get_xsize(), ball.get_ysize(), 1.0f);
    if (!started)
    {
        mat = DirectX::XMMatrixTranslation(
            player.get_xpos(),
            player.get_ypos() + ball.get_ysize(),
            0.0f);
    }
    else
    {
        mat = DirectX::XMMatrixTranslation(
            ball.get_xpos(),
            ball.get_ypos(),
            0.0f);
    }
    W = DirectX::XMMatrixMultiply(scale_mat, mat);
    wvp = W * V * P;
    BasicCB ball_cb;
    XMStoreFloat4x4(&ball_cb.wvp, XMMatrixTranspose(wvp));
    ball_cb.color = ball_color;
    cb.update(joj::Engine::s_renderer->get_device_context(), ball_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(6, 0, 0);

    draw_blocks();

    joj::Engine::s_renderer->swap_buffers();
}

void BreakoutGame::shutdown()
{
    m_input_layout->Release();
}

void BreakoutGame::draw_blocks()
{
    for (const Block& block : blocks)
    {
        if (!block.active)
            continue;

        auto scale_mat = DirectX::XMMatrixScaling(block.object.get_xsize(), block.object.get_ysize(), 1.0f);
        auto mat = DirectX::XMMatrixTranslation(block.object.get_xpos(), block.object.get_ypos(), 0.0f);
        auto W = DirectX::XMMatrixMultiply(scale_mat, mat);
        DirectX::XMVECTOR pos = DirectX::XMVectorSet(0, 0, -3, 1);
        DirectX::XMVECTOR target = DirectX::XMVectorZero();
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
        DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(pos, target, up);
        DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicOffCenterLH(
            0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f
        );
        auto wvp = W * V * P;

        BasicCB block_cb;
        XMStoreFloat4x4(&block_cb.wvp, XMMatrixTranspose(wvp));
        block_cb.color = block.color;
        cb.update(joj::Engine::s_renderer->get_device_context(), block_cb);

        joj::Engine::s_renderer->get_device_context()->DrawIndexed(6, 0, 0);
    }
}