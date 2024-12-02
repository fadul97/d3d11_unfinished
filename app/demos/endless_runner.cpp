#include "endless_runner.h"

#include "joj/jmacros.h"
#include "joj/engine.h"
#include "joj/resources/geometry/quad.h"

struct BasicCB
{
    joj::JFloat4x4 wvp;
    joj::JFloat2 tex_coord;
    joj::JFloat2 cell_size;
    i32 use_texture;
};

enum AnimState { STILL, WALKUP, WALKDOWN, WALKLEFT, WALKRIGHT };

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
    JOJ_LOG_IF_FAIL(m_player_tex.create(joj::Engine::s_renderer->get_device(), L"../../../../app/textures/Explosion.dds"));

    // Create Tileset
    JOJ_LOG_IF_FAIL(m_tileset.create(
        joj::Engine::s_renderer->get_device(),
        joj::Engine::s_renderer->get_device_context(),
        L"../../../../app/textures/Walking.png", joj::ImageType::PNG,
        55, 95, 8, 40));

    // Setup and Create Sampler States
    JOJ_LOG_IF_FAIL(m_sampler_state.create_anisotropic_state(joj::Engine::s_renderer->get_device()));
    m_sampler_state.bind_anisotropic_state(joj::Engine::s_renderer->get_device_context(), 0, 1);

    m_anim = joj::D3D11SpriteAnimation(&m_tileset, 0.06f, true);

    u32 SeqUp[8] = { 16, 17, 18, 19, 20, 21, 22, 23 };
    u32 SeqDown[8] = { 24, 25, 26, 27, 28, 29, 30, 31 };
    u32 SeqLeft[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    u32 SeqRight[8] = { 15, 14, 13, 12, 11, 10, 9, 8 };
    u32 SeqStill[1] = { 32 };

    m_anim.add(WALKUP, SeqUp, 8);
    m_anim.add(WALKDOWN, SeqDown, 8);
    m_anim.add(WALKLEFT, SeqLeft, 8);
    m_anim.add(WALKRIGHT, SeqRight, 8);
    m_anim.add(STILL, SeqStill, 1);

    m_anim_state = STILL;
}

void EndlessRunner::update(const f32 dt)
{
    if (joj::Engine::s_input->is_key_down('D'))
        m_anim_state = WALKRIGHT;
    if (joj::Engine::s_input->is_key_down('A'))
        m_anim_state = WALKLEFT;
    if (joj::Engine::s_input->is_key_down('W'))
        m_anim_state = WALKUP;
    if (joj::Engine::s_input->is_key_down('S'))
        m_anim_state = WALKDOWN;

    if (joj::Engine::s_input->is_key_up('W') &&
        joj::Engine::s_input->is_key_up('A') &&
        joj::Engine::s_input->is_key_up('S') &&
        joj::Engine::s_input->is_key_up('D'))
    {
        m_anim_state = STILL;
    }
    
    m_anim.select(m_anim_state);
    m_anim.update(dt);
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

    // m_player_tex.bind(joj::Engine::s_renderer->get_device_context(), 0, 1);
    joj::Engine::s_renderer->get_device_context()->PSSetShaderResources(0, 1, &m_tileset.m_srv);

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
    p1_cb.tex_coord = m_anim.get_tex_coord(); // Offset calculado no método update
    p1_cb.cell_size = m_anim.get_tex_size(); // Escala já definida
    cb.update(joj::Engine::s_renderer->get_device_context(), p1_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(6, 0, 0);

    joj::Engine::s_renderer->swap_buffers();
}

void EndlessRunner::shutdown()
{
}
