#include "systems/camera/free_camera.h"

joj::FreeCamera::FreeCamera()
    : m_movement_speed(20.0f), m_view_dirty(true),
    m_position(0.0f, 0.0f, 0.0f), m_right(1.0f, 0.0f, 0.0f),
    m_up(0.0f, 1.0f, 0.0f), m_target(0.0f, 0.0f, 1.0f),
    m_nearz(0.0f), m_farz(0.0f), m_aspect(0.0f),
    m_yvof(0.0f), m_near_window_height(0.0f),
    m_far_window_height(0.0f)
{
    m_view = float4x4_identity();
    m_proj = float4x4_identity();
    set_lens(0.25f * J_PI, 1.0f, 1.0f, 100.0f);
}

joj::FreeCamera::~FreeCamera()
{
}

joj::JFloat3 joj::FreeCamera::get_pos() const
{
    return m_position;
}

void joj::FreeCamera::set_pos(const f32 x, const f32 y, const f32 z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    m_view_dirty = true;
}

void joj::FreeCamera::set_pos(const JFloat3& v)
{
    m_position = v;
}

joj::JFloat3 joj::FreeCamera::get_right() const
{
    return m_right;
}

joj::JFloat3 joj::FreeCamera::get_up() const
{
    return m_up;
}

joj::JFloat3 joj::FreeCamera::get_target() const
{
    return m_target;
}

f32 joj::FreeCamera::get_nearZ() const
{
    return m_nearz;
}

f32 joj::FreeCamera::get_farZ() const
{
    return m_farz;
}

f32 joj::FreeCamera::get_aspect() const
{
    return m_aspect;
}

f32 joj::FreeCamera::get_yfov() const
{
    return m_yvof;
}

f32 joj::FreeCamera::get_xfov() const
{
    f32 half_width = 0.5 * get_near_window_width();
    return 2.0f * atan(half_width / m_nearz);
}

f32 joj::FreeCamera::get_near_window_width() const
{
    return m_aspect * m_near_window_height;
}

f32 joj::FreeCamera::get_near_window_height() const
{
    return m_near_window_height;
}

f32 joj::FreeCamera::get_far_window_width() const
{
    return m_aspect * m_far_window_height;
}

f32 joj::FreeCamera::get_far_window_height() const
{
    return m_far_window_height;
}

void joj::FreeCamera::set_lens(const f32 yfov, const f32 aspect, const f32 znear, const f32 zfar)
{
    m_yvof = yfov;
    m_aspect = aspect;
    m_nearz = znear;
    m_farz = zfar;

    m_near_window_height = 2.0f * m_nearz * tanf(0.5f * m_yvof);
    m_far_window_height = 2.0f * m_farz * tanf(0.5f * m_yvof);

    JMatrix4x4 P = DirectX::XMMatrixPerspectiveFovLH(m_yvof, m_aspect, m_nearz, m_farz);
    DirectX::XMStoreFloat4x4(&m_proj, P);
}

void joj::FreeCamera::look_at(const JFloat3& pos, const JFloat3& target, const JFloat3& world_up)
{
    JVector4 vpos = DirectX::XMLoadFloat3(&pos);
    JVector4 vtarget = DirectX::XMLoadFloat3(&target);
    JVector4 vup = DirectX::XMLoadFloat3(&world_up);

    JVector4 L = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(vtarget, vpos));
    JVector4 R = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vup, L));
    JVector4 U = DirectX::XMVector3Cross(L, R);

    DirectX::XMStoreFloat3(&m_position, vpos);
    DirectX::XMStoreFloat3(&m_target, L);
    DirectX::XMStoreFloat3(&m_right, R);
    DirectX::XMStoreFloat3(&m_up, U);

    m_view_dirty = true;
}

joj::JFloat4x4 joj::FreeCamera::get_view() const
{
    assert(!m_view_dirty);
    return m_view;
}

joj::JFloat4x4 joj::FreeCamera::get_proj() const
{
    return m_proj;
}

void joj::FreeCamera::strafe(const f32 d)
{
    JVector4 s = DirectX::XMVectorReplicate(d);
    JVector4 r = DirectX::XMLoadFloat3(&m_right);
    JVector4 p = DirectX::XMLoadFloat3(&m_position);
    
    JVector4 srp = DirectX::XMVectorMultiplyAdd(s, r, p);
    DirectX::XMStoreFloat3(&m_position, srp);

    m_view_dirty = true;
}

void joj::FreeCamera::walk(const f32 d)
{
    JVector4 s = DirectX::XMVectorReplicate(d);
    JVector4 l = DirectX::XMLoadFloat3(&m_target);
    JVector4 p = DirectX::XMLoadFloat3(&m_position);

    JVector4 slp = DirectX::XMVectorMultiplyAdd(s, l, p);
    DirectX::XMStoreFloat3(&m_position, slp);

    m_view_dirty = true;
}

void joj::FreeCamera::move(CameraMovement direction, f32 dt)
{
    const f32 velocity = m_movement_speed * dt;
    JVector4 p;
    JVector4 f;
    JVector4 r;
    switch (direction)
    {
    case joj::CameraMovement::FORWARD:
        // Convert XMFLOAT3 to XMVECTOR
        p = DirectX::XMLoadFloat3(&m_position);
        f = DirectX::XMLoadFloat3(&m_target);

        // Perform scalar multiplication and vector addition
        r = DirectX::XMVectorAdd(p, DirectX::XMVectorScale(f, velocity));

        // Store the result back in the XMFLOAT3
        DirectX::XMStoreFloat3(&m_position, r);
        break;
    case joj::CameraMovement::BACKWARD:
        // Convert XMFLOAT3 to XMVECTOR
        p = DirectX::XMLoadFloat3(&m_position);
        f = DirectX::XMLoadFloat3(&m_target);

        // Perform scalar multiplication and vector subtraction
        r = DirectX::XMVectorSubtract(p, DirectX::XMVectorScale(f, velocity));

        // Store the result back in the XMFLOAT3
        DirectX::XMStoreFloat3(&m_position, r);
        break;
    case joj::CameraMovement::LEFT:
        // Convert XMFLOAT3 to XMVECTOR
        p = DirectX::XMLoadFloat3(&m_position);
        r = DirectX::XMLoadFloat3(&m_right);

        // Perform scalar multiplication and vector addition
        r = DirectX::XMVectorAdd(p, DirectX::XMVectorScale(r, velocity));

        // Store the result back in the XMFLOAT3
        DirectX::XMStoreFloat3(&m_position, r);
        break;
    case joj::CameraMovement::RIGHT:
        // Convert XMFLOAT3 to XMVECTOR
        p = DirectX::XMLoadFloat3(&m_position);
        r = DirectX::XMLoadFloat3(&m_right);

        // Perform scalar multiplication and vector subtraction
        r = DirectX::XMVectorSubtract(p, DirectX::XMVectorScale(r, velocity));

        // Store the result back in the XMFLOAT3
        DirectX::XMStoreFloat3(&m_position, r);
        break;
    default:
        break;
    }
}

void joj::FreeCamera::pitch(const f32 angle)
{
    using namespace DirectX;
    assert(!DirectX::XMVector3Equal(XMLoadFloat3(&m_right), DirectX::XMVectorZero()));
    assert(!DirectX::XMVector3Equal(XMLoadFloat3(&m_up), DirectX::XMVectorZero()));
    assert(!DirectX::XMVector3Equal(XMLoadFloat3(&m_target), DirectX::XMVectorZero()));

    JVector4 vright = DirectX::XMLoadFloat3(&m_right);
    vright = DirectX::XMVector3Normalize(vright);
    JMatrix4x4 R = DirectX::XMMatrixRotationAxis(vright, angle);

    JVector4 vup = DirectX::XMLoadFloat3(&m_up);
    JVector4 vtarget = DirectX::XMLoadFloat3(&m_target);

    JVector4 up_normal = DirectX::XMVector3TransformNormal(vup, R);
    JVector4 target_normal = DirectX::XMVector3TransformNormal(vtarget, R);

    DirectX::XMStoreFloat3(&m_up, up_normal);
    DirectX::XMStoreFloat3(&m_target, target_normal);

    m_view_dirty = true;
}

void joj::FreeCamera::rotateY(const f32 angle)
{
    using namespace DirectX;
    assert(!DirectX::XMVector3Equal(XMLoadFloat3(&m_right), DirectX::XMVectorZero()));
    assert(!DirectX::XMVector3Equal(XMLoadFloat3(&m_up), DirectX::XMVectorZero()));
    assert(!DirectX::XMVector3Equal(XMLoadFloat3(&m_target), DirectX::XMVectorZero()));

    JMatrix4x4 R = DirectX::XMMatrixRotationY(angle);

    JVector4 vright = DirectX::XMLoadFloat3(&m_right);
    JVector4 vup = DirectX::XMLoadFloat3(&m_up);
    JVector4 vtarget = DirectX::XMLoadFloat3(&m_target);

    JVector4 right_normal = DirectX::XMVector3TransformNormal(vright, R);
    JVector4 up_normal = DirectX::XMVector3TransformNormal(vup, R);
    JVector4 target_normal = DirectX::XMVector3TransformNormal(vtarget, R);

    DirectX::XMStoreFloat3(&m_right, right_normal);
    DirectX::XMStoreFloat3(&m_up, up_normal);
    DirectX::XMStoreFloat3(&m_target, target_normal);

    m_view_dirty = true;
}

void joj::FreeCamera::update_view_matrix()
{
    if (m_view_dirty)
    {
        JVector4 R = DirectX::XMLoadFloat3(&m_right);
        JVector4 U = DirectX::XMLoadFloat3(&m_up);
        JVector4 L = DirectX::XMLoadFloat3(&m_target);
        JVector4 P = DirectX::XMLoadFloat3(&m_position);

        L = DirectX::XMVector3Normalize(L);
        U = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(L, R));

        // U, L already ortho-normal, so no need to normalize cross product.
        R = DirectX::XMVector3Cross(U, L);

        // Fill in the view matrix entries.
        using namespace DirectX;
        f32 x = -XMVectorGetX(DirectX::XMVector3Dot(P, R));
        f32 y = -XMVectorGetX(DirectX::XMVector3Dot(P, U));
        f32 z = -XMVectorGetX(DirectX::XMVector3Dot(P, L));

        DirectX::XMStoreFloat3(&m_right, R);
        DirectX::XMStoreFloat3(&m_up, U);
        DirectX::XMStoreFloat3(&m_target, L);

        m_view(0, 0) = m_right.x;
        m_view(1, 0) = m_right.y;
        m_view(2, 0) = m_right.z;
        m_view(3, 0) = x;

        m_view(0, 1) = m_up.x;
        m_view(1, 1) = m_up.y;
        m_view(2, 1) = m_up.z;
        m_view(3, 1) = y;

        m_view(0, 2) = m_target.x;
        m_view(1, 2) = m_target.y;
        m_view(2, 2) = m_target.z;
        m_view(3, 2) = z;

        m_view(0, 3) = 0.0f;
        m_view(1, 3) = 0.0f;
        m_view(2, 3) = 0.0f;
        m_view(3, 3) = 1.0f;

        m_view_dirty = false;
    }
}