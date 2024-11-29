#include "pong3d.h"

#include "joj/engine.h"
#include "joj/logger.h"
#include "joj/resources/geometry/cube.h"
#include "joj/math/jmath.h"

struct BasicCB
{
    joj::JFloat4x4 wvp;
};

void Pong3DGame::init()
{
    m_camera.set_pos(0.0f, 2.0f, -15.0f);
    m_camera.set_lens(0.25f * J_PI, 800.0f / 600.0f, 1.0f, 1000.0f);

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

    joj::Cube cube(1.0f, 1.0f, 1.0f);

    m_cube_index_count = cube.get_index_count();

    // Setup and Create Vertex Buffer
    vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * cube.get_vertex_count(), cube.get_vertex_data());
    if (joj::Engine::s_renderer->get_device()->CreateBuffer(
        vb.get_buffer_desc(),
        vb.get_subdata(),
        &vb.get_buffer()) != S_OK)
    {
        JERROR(joj::ErrorCode::FAILED, "Failed to create vertex buffer.");
    }

    // Setup and Create Index Buffer
    ib.setup(sizeof(u32) * cube.get_index_count(), cube.get_index_data());
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
}

void Pong3DGame::update(const f32 dt)
{
    if (joj::Engine::s_input->is_key_down('W'))
        m_camera.walk(dt * 35.0f);

    if (joj::Engine::s_input->is_key_down(joj::KEY_S))
        m_camera.walk(dt * -35.0f);

    if (joj::Engine::s_input->is_key_down(joj::KEY_A))
        m_camera.strafe(dt * -35.0f);

    if (joj::Engine::s_input->is_key_down(joj::KEY_D))
        m_camera.strafe(dt * 35.0f);

    m_camera.update_view_matrix();

    i32 top_max = 11;
    i32 top_min = -11;
    i32 right = 13;
    i32 left = -13;
    if (joj::Engine::s_input->is_key_down('K'))
        p1.translate_ypos(player_velocity * dt);
    if (joj::Engine::s_input->is_key_down('J'))
        p1.translate_ypos(-player_velocity * dt);

    if (joj::Engine::s_input->is_key_down(joj::KEY_UP))
        p2.translate_ypos(player_velocity * dt);
    if (joj::Engine::s_input->is_key_down(joj::KEY_DOWN))
        p2.translate_ypos(-player_velocity * dt);

    ball.translate_xpos(ball_velocity.x * dt);
    ball.translate_ypos(ball_velocity.y * dt);

    // Player 1
    if (p1.get_ypos() + p1.get_ysize() / 2 >= top_max)
        p1.move_to(p1.get_xpos(), top_max - p1.get_ysize() / 2);

    if (p1.get_ypos() - p1.get_ysize() / 2 <= top_min)
        p1.move_to(p1.get_xpos(), top_min + p1.get_ysize() / 2);

    // Player 2
    if (p2.get_ypos() + p2.get_ysize() / 2 >= top_max)
        p2.move_to(p2.get_xpos(), top_max - p2.get_ysize() / 2);

    if (p2.get_ypos() - p2.get_ysize() / 2 <= top_min)
        p2.move_to(p2.get_xpos(), top_min + p2.get_ysize() / 2);

    // Ball Y-axis
    if (ball.get_ypos() + ball.get_ysize() / 2 >= top_max)
    {
        ball.move_to(ball.get_xpos(), top_max - ball.get_ysize() / 2);
        ball_velocity.y *= -1.0f;
    }

    if (ball.get_ypos() - ball.get_ysize() / 2 <= top_min)
    {
        ball.move_to(ball.get_xpos(), top_min + ball.get_ysize() / 2);
        ball_velocity.y *= -1.0f;
    }

    // End Game
    if (ball.get_xpos() + ball.get_xsize() / 2 >= right)
    {
        ball.move_to(right - ball.get_xsize() / 2, ball.get_ypos());
        ball_velocity.x = 0.0f;
        ball_velocity.y = 0.0f;
        player_velocity = 0.0f;
    }

    if (ball.get_xpos() - ball.get_xsize() / 2 <= left)
    {
        ball.move_to(left + ball.get_xsize() / 2, ball.get_ypos());
        ball_velocity.x = 0.0f;
        ball_velocity.y = 0.0f;
        player_velocity = 0.0f;
    }

    // Ball and Player 1 collision
    if (ball.get_xpos() + ball.get_xsize() / 2 >= p1.get_xpos() - p1.get_xsize() / 2 &&
        ball.get_xpos() - ball.get_xsize() / 2 <= p1.get_xpos() + p1.get_xsize() / 2 &&
        ball.get_ypos() + ball.get_ysize() / 2 >= p1.get_ypos() - p1.get_ysize() / 2 &&
        ball.get_ypos() - ball.get_ysize() / 2 <= p1.get_ypos() + p1.get_ysize() / 2)
    {
        ball.move_to(p1.get_xpos() + p1.get_xsize(), ball.get_ypos());
        ball_velocity.x *= -1.0f;
    }

    // Ball and Player 2 collision
    if (ball.get_xpos() -  ball.get_xsize() / 2 <= p2.get_xpos() + p2.get_xsize() / 2 &&
        ball.get_xpos() + ball.get_xsize() / 2 >= p2.get_xpos() - p2.get_xsize() / 2 &&
        ball.get_ypos() >= p2.get_ypos() - p2.get_ysize() / 2 &&
        ball.get_ypos() <= p2.get_ypos() + p2.get_ysize() / 2)
    {
        ball.move_to(p2.get_xpos() - p2.get_xsize(), ball.get_ypos());
        ball_velocity.x *= -1.0f;
    }
}

void Pong3DGame::draw()
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
    auto scale_mat = DirectX::XMMatrixScaling(p1.get_xsize(), p1.get_ysize(), 1.0f);
    auto mat = DirectX::XMMatrixTranslation(p1.get_xpos(), p1.get_ypos(), 0.0f);
    auto W = DirectX::XMMatrixMultiply(scale_mat, mat);
    joj::JFloat4x4 camera_view = m_camera.get_view();
    joj::JFloat4x4 camera_proj = m_camera.get_proj();
    auto V = DirectX::XMLoadFloat4x4(&camera_view);
    auto P = DirectX::XMLoadFloat4x4(&camera_proj);
    auto wvp = W * V * P;
    BasicCB p1_cb;
    XMStoreFloat4x4(&p1_cb.wvp, XMMatrixTranspose(wvp));
    cb.update(joj::Engine::s_renderer->get_device_context(), p1_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_cube_index_count, 0, 0);

    scale_mat = DirectX::XMMatrixScaling(p2.get_xsize(), p2.get_ysize(), 1.0f);
    mat = DirectX::XMMatrixTranslation(p2.get_xpos(), p2.get_ypos(), 0.0f);
    W = DirectX::XMMatrixMultiply(scale_mat, mat);
    wvp = W * V * P;
    BasicCB p2_cb;
    XMStoreFloat4x4(&p2_cb.wvp, XMMatrixTranspose(wvp));
    cb.update(joj::Engine::s_renderer->get_device_context(), p2_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_cube_index_count, 0, 0);

    scale_mat = DirectX::XMMatrixScaling(ball.get_xsize(), ball.get_ysize(), 1.0f);
    mat = DirectX::XMMatrixTranslation(ball.get_xpos(), ball.get_ypos(), 0.0f);
    W = DirectX::XMMatrixMultiply(scale_mat, mat);
    wvp = W * V * P;
    BasicCB ball_cb;
    XMStoreFloat4x4(&ball_cb.wvp, XMMatrixTranspose(wvp));
    cb.update(joj::Engine::s_renderer->get_device_context(), ball_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(m_cube_index_count, 0, 0);

    joj::Engine::s_renderer->swap_buffers();
}

void Pong3DGame::shutdown()
{
    m_input_layout->Release();
}

void Pong3DGame::on_mouse_down(joj::Buttons button, i32 x, i32 y)
{
    m_last_mouse_pos.x = x;
    m_last_mouse_pos.y = y;

    SetCapture(joj::Engine::s_window->get_window_data().handle);
}

void Pong3DGame::on_mouse_up(joj::Buttons button, i32 x, i32 y)
{
    ReleaseCapture();
}

void Pong3DGame::on_mouse_move(WPARAM button_state, i32 x, i32 y)
{
    if ((button_state & MK_RBUTTON) != 0)
    {
        f32 dx = DirectX::XMConvertToRadians(0.25f * static_cast<f32>(x - m_last_mouse_pos.x));
        f32 dy = DirectX::XMConvertToRadians(0.25f * static_cast<f32>(y - m_last_mouse_pos.y));

        m_camera.pitch(dy);
        m_camera.rotateY(dx);
    }

    m_last_mouse_pos.x = x;
    m_last_mouse_pos.y = y;
}