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
        FreeCamera();
        ~FreeCamera();

        JFloat3 get_pos() const;
        void set_pos(const f32 x, const f32 y, const f32 z);
        void set_pos(const JFloat3& v);

        JFloat3 get_right() const;
        JFloat3 get_up() const;
        JFloat3 get_target() const;

        f32 get_nearZ() const;
        f32 get_farZ() const;
        f32 get_aspect() const;
        f32 get_yfov() const;
        f32 get_xfov() const;

        f32 get_near_window_width() const;
        f32 get_near_window_height() const;
        f32 get_far_window_width() const;
        f32 get_far_window_height() const;

        void set_lens(const f32 yfov, const f32 aspect, const f32 znear, const f32 zfar);

        void look_at(const JFloat3& pos, const JFloat3& target, const JFloat3& world_up);

        JFloat4x4 get_view() const;
        JFloat4x4 get_proj() const;

        void strafe(const f32 d);
        void walk(const f32 d);

        // FIXME: 
        void move(CameraMovement direction, f32 dt);

        void pitch(const f32 angle);
        void rotateY(const f32 angle);

        void update_view_matrix();

    private:
        JFloat3 m_position;
        JFloat3 m_right;
        JFloat3 m_up;
        JFloat3 m_target;

        f32 m_nearz;
        f32 m_farz;
        f32 m_aspect;
        f32 m_yvof;
        f32 m_near_window_height;
        f32 m_far_window_height;

        b8 m_view_dirty;

        JFloat4x4 m_view;
        JFloat4x4 m_proj;

        f32 m_movement_speed;
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_FREE_CAMERA_H