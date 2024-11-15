#ifndef JOJ_FREE_CAMERA_H
#define JOJ_FREE_CAMERA_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "math/jmath.h"

namespace joj
{
    enum class CameraMovement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    const f32 YAW = -90.0f;
    const f32 PITCH = 0.0f;
    const f32 SPEED = 10.0f;
    const f32 SENSITIVITY = 0.1f;
    const f32 ZOOM = 45.0f;

    class JAPI FreeCamera
    {
    public:
        FreeCamera(JFloat3 pos = JFloat3{ 0.0f, 0.0f, 0.0f }, JFloat3 up = JFloat3{ 0.0f, 1.0f, 0.0f }, f32 yaw = YAW, f32 pitch = PITCH);
        ~FreeCamera();

        JMatrix4x4 get_view_mat() const;

        void process_keyboard(CameraMovement direction, f32 dt);
        void process_mouse_movement(f32 xoffset, f32 yoffset, b8 constrain_pitch = true);
        void process_mouse_scroll(f32 yoffset);

    public:
        JFloat3 m_position;
        JFloat3 m_target;
        JFloat3 m_up;
        JFloat3 m_right;
        JFloat3 m_world_up;

        f32 m_yaw;
        f32 m_pitch;

        f32 m_movement_speed;
        f32 m_mouse_sensitivity;
        f32 m_zoom;

        void update_camera_vectors();
    };

    inline JMatrix4x4 FreeCamera::get_view_mat() const
    {
        JFloat3 r = JFloat3{ m_position.x + m_target.x, m_position.y + m_target.y, m_position.z + m_target.z };
        return DirectX::XMMatrixLookAtLH(
            DirectX::XMLoadFloat3(&m_position),
            DirectX::XMLoadFloat3(&r),
            DirectX::XMLoadFloat3(&m_up)
        );
    }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_FREE_CAMERA_H