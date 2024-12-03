#include "endless_runner.h"

#include "joj/jmacros.h"
#include "joj/engine.h"
#include "joj/resources/geometry/quad.h"

struct BasicCB
{
    joj::JFloat4x4 wvp;
    joj::JFloat2 uv_offset;
    joj::JFloat2 cell_size;
    i32 use_texture;
};

void EndlessRunner::init()
{
    // Create Shader
    m_shader.compile_vertex_shader(L"../../../../app/shaders/SpriteAnimation.hlsl", "VS", "vs_5_0");
    m_shader.compile_pixel_shader(L"../../../../app/shaders/SpriteAnimation.hlsl", "PS", "ps_5_0");
    JOJ_LOG_IF_FAIL(m_shader.create_vertex_shader(joj::Engine::s_renderer->get_device()));
    JOJ_LOG_IF_FAIL(m_shader.create_pixel_shader(joj::Engine::s_renderer->get_device()));

    // Setup and Create Input Layout
    m_input_layout.describe_default_geometry_layout();
    JOJ_LOG_IF_FAIL(m_input_layout.create(joj::Engine::s_renderer->get_device(), m_shader));

    joj::Quad quad(1.0f, 1.0f);

    // Setup and Create Vertex Buffer
    vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * quad.get_vertex_count(), quad.get_vertex_data());
    JOJ_LOG_IF_FAIL(vb.create(joj::Engine::s_renderer->get_device()));

    // Setup and Create Index Buffer
    ib.setup(sizeof(u32) * quad.get_index_count(), quad.get_index_data());
    JOJ_LOG_IF_FAIL(ib.create(joj::Engine::s_renderer->get_device()));

    // Setup and Create Constant Buffer
    cb.setup(joj::calculate_cb_byte_size(sizeof(BasicCB)), nullptr);
    JOJ_LOG_IF_FAIL(cb.create(joj::Engine::s_renderer->get_device()));

    // Create Textures
    JOJ_LOG_IF_FAIL(m_fire_ss.create(
        joj::Engine::s_renderer->get_device(),
        L"../../../../app/textures/fire_animation_atlas.dds",
        2560, 3072, 12, 10));

    JOJ_LOG_IF_FAIL(m_ship_ss.create(
        joj::Engine::s_renderer->get_device(),
        L"../../../../app/textures/shipanimated.dds",
        256, 64, 1, 4));

    JOJ_LOG_IF_FAIL(m_guy_ss.create(
        joj::Engine::s_renderer->get_device(),
        L"../../../../app/textures/GravityGuy.dds",
        160, 96, 2, 5));

    // Setup and Create Sampler States
    JOJ_LOG_IF_FAIL(m_sampler_state.create_pixel_state(joj::Engine::s_renderer->get_device()));
    m_sampler_state.bind_pixel_state(joj::Engine::s_renderer->get_device_context(), 0, 1);

    m_fire_animation = joj::D3D11SpriteAnimation{ 120, true };
    m_ship_animation = joj::D3D11SpriteAnimation{ 4, true };
    m_guy = joj::D3D11SpriteAnimation{ 4, true };

    std::vector<joj::D3D11AnimationFrame> frames;
    
    for (i32 i = 0; i < 4; ++i)
    {
        joj::D3D11AnimationFrame frame = joj::D3D11AnimationFrame(m_ship_ss, i, 0);
        frames.push_back(frame);
    }

    m_ship_animation.add_frame(0, frames.data(), 4);
    frames.clear();

    for (i32 i = 0; i < 12; ++i)
    {
        for (i32 j = 0; j < 10; ++j)
        {
            joj::D3D11AnimationFrame frame = joj::D3D11AnimationFrame(m_fire_ss, j, i);
            frames.push_back(frame);
        }
    }

    m_fire_animation.add_frame(0, frames.data(), 120);
    frames.clear();

    for (i32 i = 1; i < 5; ++i)
    {
        joj::D3D11AnimationFrame frame = joj::D3D11AnimationFrame(m_guy_ss, i, 0);
        frames.push_back(frame);
    }

    m_guy.add_frame(WALKRIGHT, frames.data(), 4);
    frames.clear();

    for (i32 i = 1; i < 5; ++i)
    {
        joj::D3D11AnimationFrame frame = joj::D3D11AnimationFrame(m_guy_ss, i, 1);
        frames.push_back(frame);
    }

    m_guy.add_frame(WALKUPSIDE, frames.data(), 4);
    frames.clear();

    joj::D3D11AnimationFrame frame1 = joj::D3D11AnimationFrame(m_guy_ss, 0, 0);
    m_guy.add_frame(STILL, &frame1, 1);

    joj::D3D11AnimationFrame frame2 = joj::D3D11AnimationFrame(m_guy_ss, 0, 1);
    m_guy.add_frame(UPSIDE, &frame2, 1);
}

void EndlessRunner::update(const f32 dt)
{
    m_fire_animation.update(dt);
    m_ship_animation.update(dt);

    if (joj::Engine::s_input->is_key_pressed('W'))
        m_guy_inverted = !m_guy_inverted;

    if (joj::Engine::s_input->is_key_down('D'))
    {
        if (m_guy_inverted)
            m_state = WALKUPSIDE;
        else
            m_state = WALKRIGHT;
    }

    if (joj::Engine::s_input->is_key_up('D') &&
        joj::Engine::s_input->is_key_up('W') &&
        joj::Engine::s_input->is_key_up('E'))
    {
        if (m_guy_inverted)
            m_state = UPSIDE;
        else
            m_state = STILL;
    }

    m_guy.select(m_state);
    m_guy.update(dt);
}

void EndlessRunner::draw()
{
    joj::Engine::s_renderer->clear();

    m_input_layout.bind(joj::Engine::s_renderer->get_device_context());
    joj::Engine::s_renderer->set_primitive_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    u32 stride = sizeof(joj::GeometryVertex);
    u32 offset = 0;
    vb.bind(joj::Engine::s_renderer->get_device_context(), 0, 1, &stride, &offset);
    ib.bind(joj::Engine::s_renderer->get_device_context(), DXGI_FORMAT_R32_UINT, offset);

    m_shader.bind_vertex_shader(joj::Engine::s_renderer->get_device_context());
    m_shader.bind_pixel_shader(joj::Engine::s_renderer->get_device_context());

    cb.bind_to_vertex_shader(joj::Engine::s_renderer->get_device_context(), 0, 1);
    cb.bind_to_pixel_shader(joj::Engine::s_renderer->get_device_context(), 0, 1);

    m_fire_ss.bind(joj::Engine::s_renderer->get_device_context(), 0, 1);

    auto scale_mat = DirectX::XMMatrixScaling(250.0f, 250.0f, 1.0f);
    auto mat = DirectX::XMMatrixTranslation(250.0f, 300.0f, 0.0f);
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
    p1_cb.use_texture = 1;
    p1_cb.uv_offset = m_fire_animation.get_tex_coord();
    p1_cb.cell_size = m_fire_animation.get_cell_size();
    cb.update(joj::Engine::s_renderer->get_device_context(), p1_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(6, 0, 0);

    m_ship_ss.bind(joj::Engine::s_renderer->get_device_context(), 0, 1);

    scale_mat = DirectX::XMMatrixScaling(250.0f, 250.0f, 1.0f);
    mat = DirectX::XMMatrixTranslation(600.0f, 400.0f, 0.0f);
    W = DirectX::XMMatrixMultiply(scale_mat, mat);
    wvp = W * V * P;
    XMStoreFloat4x4(&p1_cb.wvp, XMMatrixTranspose(wvp));
    p1_cb.use_texture = 1;
    p1_cb.uv_offset = m_ship_animation.get_tex_coord();
    p1_cb.cell_size = m_ship_animation.get_cell_size();
    cb.update(joj::Engine::s_renderer->get_device_context(), p1_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(6, 0, 0);

    m_guy_ss.bind(joj::Engine::s_renderer->get_device_context(), 0, 1);

    scale_mat = DirectX::XMMatrixScaling(250.0f, 250.0f, 1.0f);
    mat = DirectX::XMMatrixTranslation(600.0f, 100.0f, 0.0f);
    W = DirectX::XMMatrixMultiply(scale_mat, mat);
    wvp = W * V * P;
    XMStoreFloat4x4(&p1_cb.wvp, XMMatrixTranspose(wvp));
    p1_cb.use_texture = 1;
    p1_cb.uv_offset = m_guy.get_tex_coord();
    p1_cb.cell_size = m_guy.get_cell_size();
    cb.update(joj::Engine::s_renderer->get_device_context(), p1_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(6, 0, 0);

    joj::Engine::s_renderer->swap_buffers();
}

void EndlessRunner::shutdown()
{
}
