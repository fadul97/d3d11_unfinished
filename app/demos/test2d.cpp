#include "test2d.h"

#include "joj/engine.h"
#include "joj/logger.h"
#include "joj/math/jmath.h"

struct BasicCB
{
    joj::JFloat4x4 wvp;
};

void Test2D::init()
{
    p1.move_to(0, 0, -10);

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

    // Setup and Create Vertex Buffer
    vb.setup(D3D11_USAGE_IMMUTABLE, 0, sizeof(joj::GeometryVertex) * p1.get_vertex_count(), p1.get_vertex_data());
    if (joj::Engine::s_renderer->get_device()->CreateBuffer(
        vb.get_buffer_desc(),
        vb.get_subdata(),
        &vb.get_buffer()) != S_OK)
    {
        JERROR(joj::ErrorCode::FAILED, "Failed to create vertex buffer.");
    }

    // Setup and Create Index Buffer
    ib.setup(sizeof(u32) * p1.get_index_count(), p1.get_index_data());
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

void Test2D::update(const f32 dt)
{
}

void Test2D::draw()
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
    auto mat = DirectX::XMMatrixTranslation(0, 0, 0.0f);
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(0, 0, -3, 1);
    DirectX::XMVECTOR target = DirectX::XMVectorZero();
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
    DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(pos, target, up);
    constexpr f32 fov_angle = 45;
    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XMConvertToRadians(fov_angle), 800.0f / 600.0f, 0.1f, 100.0f
    );
    auto wvp = mat * V * P;
    BasicCB p1_cb;
    XMStoreFloat4x4(&p1_cb.wvp, XMMatrixTranspose(wvp));
    cb.update(joj::Engine::s_renderer->get_device_context(), p1_cb);

    joj::Engine::s_renderer->get_device_context()->DrawIndexed(6, 0, 0);

    joj::Engine::s_renderer->swap_buffers();
}

void Test2D::shutdown()
{
    m_input_layout->Release();
}
