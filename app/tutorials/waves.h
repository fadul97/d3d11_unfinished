#ifndef WAVES_H
#define WAVES_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <vector>
#include "joj/math/jmath.h"

class Waves
{
public:
    Waves(i32 m, i32 n, f32 dx, f32 dt, f32 speed, f32 damping);
    Waves(const Waves& rhs) = delete;
    Waves& operator=(const Waves& rhs) = delete;
    ~Waves();

    i32 get_row_count() const;
    i32 get_column_count() const;
    i32 get_vertex_count() const;
    i32 get_triangle_count() const;
    f32 get_width() const;
    f32 get_depth() const;

    // Returns the solution at the ith grid poi32.
    const joj::JFloat3& Position(i32 i) const { return m_curr_solution[i]; }

    // Returns the solution normal at the ith grid poi32.
    const joj::JFloat3& Normal(i32 i) const { return m_normals[i]; }

    // Returns the unit tangent vector at the ith grid poi32 in the local x-axis direction.
    const joj::JFloat3& TangentX(i32 i) const { return m_tangentX[i]; }

    void update(f32 dt);
    void disturb(i32 i, i32 j, f32 magnitude);

private:
    i32 m_num_rows = 0;
    i32 m_num_cols = 0;

    i32 m_vertex_count = 0;
    i32 m_triangle_count = 0;

    // Simulation constants we can precompute.
    f32 m_k1 = 0.0f;
    f32 m_k2 = 0.0f;
    f32 m_k3 = 0.0f;

    f32 m_time_step = 0.0f;
    f32 m_spatial_step = 0.0f;

    std::vector<joj::JFloat3> m_prev_solution;
    std::vector<joj::JFloat3> m_curr_solution;
    std::vector<joj::JFloat3> m_normals;
    std::vector<joj::JFloat3> m_tangentX;
};

#endif // JPLATFORM_WINDOWS

#endif // WAVES_H