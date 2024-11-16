#include "waves.h"

#if JPLATFORM_WINDOWS

#include <ppl.h>
#include <algorithm>
#include <vector>
#include <cassert>

using namespace DirectX;

Waves::Waves(i32 m, i32 n, f32 dx, f32 dt, f32 speed, f32 damping)
{
	m_num_rows = m;
	m_num_cols = n;

	m_vertex_count = m * n;
	m_triangle_count = (m - 1) * (n - 1) * 2;

	m_time_step = dt;
	m_spatial_step = dx;

	f32 d = damping * dt + 2.0f;
	f32 e = (speed * speed) * (dt * dt) / (dx * dx);
	m_k1 = (damping * dt - 2.0f) / d;
	m_k2 = (4.0f - 8.0f * e) / d;
	m_k3 = (2.0f * e) / d;

	m_prev_solution.resize(m * n);
	m_curr_solution.resize(m * n);
	m_normals.resize(m * n);
	m_tangentX.resize(m * n);

	// Generate grid vertices in system memory.

	f32 halfWidth = (n - 1) * dx * 0.5f;
	f32 halfDepth = (m - 1) * dx * 0.5f;
	for (i32 i = 0; i < m; ++i)
	{
		f32 z = halfDepth - i * dx;
		for (i32 j = 0; j < n; ++j)
		{
			f32 x = -halfWidth + j * dx;

			m_prev_solution[i * n + j] = XMFLOAT3(x, 0.0f, z);
			m_curr_solution[i * n + j] = XMFLOAT3(x, 0.0f, z);
			m_normals[i * n + j] = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_tangentX[i * n + j] = XMFLOAT3(1.0f, 0.0f, 0.0f);
		}
	}
}

Waves::~Waves()
{
}

i32 Waves::get_row_count() const
{
	return m_num_rows;
}

i32 Waves::get_column_count() const
{
	return m_num_cols;
}

i32 Waves::get_vertex_count() const
{
	return m_vertex_count;
}

i32 Waves::get_triangle_count() const
{
	return m_triangle_count;
}

f32 Waves::get_width() const
{
	return m_num_cols * m_spatial_step;
}

f32 Waves::get_depth() const
{
	return m_num_rows * m_spatial_step;
}

void Waves::update(f32 dt)
{
	static f32 t = 0;

	// Accumulate time.
	t += dt;

	// Only update the simulation at the specified time step.
	if (t >= m_time_step)
	{
		// Only update interior points; we use zero boundary conditions.
		concurrency::parallel_for(1, m_num_rows - 1, [this](i32 i)
			// for (i32 i = 1; i < m_num_rows - 1; ++i)
			{
				for (i32 j = 1; j < m_num_cols - 1; ++j)
				{
					// After this update we will be discarding the old previous
					// buffer, so overwrite that buffer with the new update.
					// Note how we can do this inplace (read/write to same element) 
					// because we won't need prev_ij again and the assignment happens last.

					// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
					// Moreover, our +z axis goes "down"; this is just to 
					// keep consistent with our row indices going down.

					m_prev_solution[i * m_num_cols + j].y =
						m_k1 * m_prev_solution[i * m_num_cols + j].y +
						m_k2 * m_curr_solution[i * m_num_cols + j].y +
						m_k3 * (m_curr_solution[(i + 1) * m_num_cols + j].y +
							m_curr_solution[(i - 1) * m_num_cols + j].y +
							m_curr_solution[i * m_num_cols + j + 1].y +
							m_curr_solution[i * m_num_cols + j - 1].y);
				}
			});

		// We just overwrote the previous buffer with the new data, so
		// this data needs to become the current solution and the old
		// current solution becomes the new previous solution.
		std::swap(m_prev_solution, m_curr_solution);

		t = 0.0f; // reset time

		//
		// Compute normals using finite difference scheme.
		//
		concurrency::parallel_for(1, m_num_rows - 1, [this](i32 i)
			//for(int i = 1; i < mNumRows - 1; ++i)
			{
				for (i32 j = 1; j < m_num_cols - 1; ++j)
				{
					f32 l = m_curr_solution[i * m_num_cols + j - 1].y;
					f32 r = m_curr_solution[i * m_num_cols + j + 1].y;
					f32 t = m_curr_solution[(i - 1) * m_num_cols + j].y;
					f32 b = m_curr_solution[(i + 1) * m_num_cols + j].y;
					m_normals[i * m_num_cols + j].x = -r + l;
					m_normals[i * m_num_cols + j].y = 2.0f * m_spatial_step;
					m_normals[i * m_num_cols + j].z = b - t;

					joj::JVector4 n = XMVector3Normalize(XMLoadFloat3(&m_normals[i * m_num_cols + j]));
					XMStoreFloat3(&m_normals[i * m_num_cols + j], n);

					m_tangentX[i * m_num_cols + j] = XMFLOAT3(2.0f * m_spatial_step, r - l, 0.0f);
					joj::JVector4 T = XMVector3Normalize(XMLoadFloat3(&m_tangentX[i * m_num_cols + j]));
					XMStoreFloat3(&m_tangentX[i * m_num_cols + j], T);
				}
			});
	}
}

void Waves::disturb(i32 i, i32 j, f32 magnitude)
{
	// TODO: Make sure assert works
	// Don't disturb boundaries.
	assert(i > 1 && i < m_num_rows - 2);
	assert(j > 1 && j < m_num_cols - 2);

	f32 halfMag = 0.5f * magnitude;

	// Disturb the ijth vertex height and its neighbors.
	m_curr_solution[i * m_num_cols + j].y += magnitude;
	m_curr_solution[i * m_num_cols + j + 1].y += halfMag;
	m_curr_solution[i * m_num_cols + j - 1].y += halfMag;
	m_curr_solution[(i + 1) * m_num_cols + j].y += halfMag;
	m_curr_solution[(i - 1) * m_num_cols + j].y += halfMag;
}

#endif // JPLATFORM_WINDOWS