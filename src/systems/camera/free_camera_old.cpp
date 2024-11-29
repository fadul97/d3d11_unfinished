#include "systems/camera/free_camera_old.h"

#if JPLATFORM_WINDOWS

joj::FreeCameraOld::FreeCameraOld(JFloat3 pos, JFloat3 up, f32 yaw, f32 pitch)
    : m_target(JFloat3{ 0.0f, 0.0f, 0.0f }), m_movement_speed(SPEED), m_mouse_sensitivity(SENSITIVITY), m_zoom(ZOOM)
{
    m_position = pos;
    m_world_up = up;
    m_yaw = yaw;
    m_pitch = pitch;
    update_camera_vectors();
}

joj::FreeCameraOld::~FreeCameraOld()
{
}

void joj::FreeCameraOld::process_keyboard(CameraMovement direction, f32 dt)
{
    const f32 velocity = m_movement_speed * dt;
    JVector4 p;
    JVector4 f;
    JVector4 r;
    if (direction == CameraMovement::FORWARD)
    {
        // Convert XMFLOAT3 to XMVECTOR
        p = DirectX::XMLoadFloat3(&m_position);
        f = DirectX::XMLoadFloat3(&m_target);

        // Perform scalar multiplication and vector addition
        r = DirectX::XMVectorAdd(p, DirectX::XMVectorScale(f, velocity));

        // Store the result back in the XMFLOAT3
        DirectX::XMStoreFloat3(&m_position, r);
    }
    if (direction == CameraMovement::BACKWARD)
    {
        // Convert XMFLOAT3 to XMVECTOR
        p = DirectX::XMLoadFloat3(&m_position);
        f = DirectX::XMLoadFloat3(&m_target);

        // Perform scalar multiplication and vector subtraction
        r = DirectX::XMVectorSubtract(p, DirectX::XMVectorScale(f, velocity));

        // Store the result back in the XMFLOAT3
        DirectX::XMStoreFloat3(&m_position, r);
    }
    if (direction == CameraMovement::LEFT)
    {
        // Convert XMFLOAT3 to XMVECTOR
        p = DirectX::XMLoadFloat3(&m_position);
        r = DirectX::XMLoadFloat3(&m_right);

        // Perform scalar multiplication and vector addition
        r = DirectX::XMVectorAdd(p, DirectX::XMVectorScale(r, velocity));

        // Store the result back in the XMFLOAT3
        DirectX::XMStoreFloat3(&m_position, r);
    }
    if (direction == CameraMovement::RIGHT)
    {
        // Convert XMFLOAT3 to XMVECTOR
        p = DirectX::XMLoadFloat3(&m_position);
        r = DirectX::XMLoadFloat3(&m_right);

        // Perform scalar multiplication and vector subtraction
        r = DirectX::XMVectorSubtract(p, DirectX::XMVectorScale(r, velocity));

        // Store the result back in the XMFLOAT3
        DirectX::XMStoreFloat3(&m_position, r);
    }
}

void joj::FreeCameraOld::process_mouse_movement(f32 xoffset, f32 yoffset, b8 constrain_pitch)
{
    xoffset *= m_mouse_sensitivity;
    yoffset *= m_mouse_sensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrain_pitch)
    {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    update_camera_vectors();
}

void joj::FreeCameraOld::process_mouse_scroll(f32 yoffset)
{
    m_zoom -= yoffset;
    if (m_zoom < 1.0f)
        m_zoom = 1.0f;
    if (m_zoom > 45.0f)
        m_zoom = 45.0f;
}

void joj::FreeCameraOld::update_camera_vectors()
{
    // TODO: Check why f.x needs to be negated, X-axis movement works correctly
    // Create f vector
    JFloat3 f = JFloat3{ 1.0f, 1.0f, 1.0f };
    f.x = cos(DirectX::XMConvertToRadians(m_yaw)) * cos(DirectX::XMConvertToRadians(m_pitch)) * -1;
    f.y = sin(DirectX::XMConvertToRadians(m_pitch));
    f.z = sin(DirectX::XMConvertToRadians(m_yaw)) * cos(DirectX::XMConvertToRadians(m_pitch));

    // Normalize fv vector
    JVector4 fv = DirectX::XMLoadFloat3(&f);
    fv = DirectX::XMVector3Normalize(fv);

    // Store result in front member
    DirectX::XMStoreFloat3(&m_target, fv);

    // Calculate right and up vectors
    JVector4 wupv = DirectX::XMLoadFloat3(&m_world_up);
    JVector4 rv = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(fv, wupv));
    JVector4 uv = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(rv, fv));

    // Store the results back in the XMFLOAT3 members
    DirectX::XMStoreFloat3(&m_right, rv);
    DirectX::XMStoreFloat3(&m_up, uv);
}

#endif // JPLATFORM_WINDOWS