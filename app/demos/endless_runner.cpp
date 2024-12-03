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
    JOJ_LOG_IF_FAIL(m_player_tex.create(joj::Engine::s_renderer->get_device(), L"../../../../app/textures/fire_animation_atlas.dds"));

    // Setup and Create Sampler States
    JOJ_LOG_IF_FAIL(m_sampler_state.create_anisotropic_state(joj::Engine::s_renderer->get_device()));
    m_sampler_state.bind_anisotropic_state(joj::Engine::s_renderer->get_device_context(), 0, 1);

    m_fire_animation = joj::D3D11SpriteAnimation(120, 10, 12, 1.0f / 30.0f); // 30 FPS
}

void EndlessRunner::update(const f32 dt)
{
    m_fire_animation.update(dt);
}


void EndlessRunner::draw()
{
    joj::Engine::s_renderer->clear();

    m_input_layout.bind(joj::Engine::s_renderer->get_device_context());
    joj::Engine::s_renderer->set_primitive_topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(joj::GeometryVertex);
    UINT offset = 0;
    vb.bind(joj::Engine::s_renderer->get_device_context(), 0, 1, &stride, &offset);
    ib.bind(joj::Engine::s_renderer->get_device_context(), DXGI_FORMAT_R32_UINT, offset);

    m_shader.bind_vertex_shader(joj::Engine::s_renderer->get_device_context());
    m_shader.bind_pixel_shader(joj::Engine::s_renderer->get_device_context());

    cb.bind_to_vertex_shader(joj::Engine::s_renderer->get_device_context(), 0, 1);
    cb.bind_to_pixel_shader(joj::Engine::s_renderer->get_device_context(), 0, 1);

    m_player_tex.bind(joj::Engine::s_renderer->get_device_context(), 0, 1);

    auto scale_mat = DirectX::XMMatrixScaling(100.0f, 100.0f, 1.0f);
    auto mat = DirectX::XMMatrixTranslation(400.0f, 300.0f, 0.0f);
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
    p1_cb.uv_offset = m_fire_animation.get_uv_offset(); // Offset calculado no método update
    p1_cb.cell_size = m_fire_animation.get_cell_size(); // Escala já definida
    cb.update(joj::Engine::s_renderer->get_device_context(), p1_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(6, 0, 0);

    joj::Engine::s_renderer->swap_buffers();
}

void EndlessRunner::shutdown()
{
}
