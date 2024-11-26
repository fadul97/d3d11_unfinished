#ifndef JOJ_D3D11_RENDER_STATE_H
#define JOJ_D3D11_RENDER_STATE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "error_code.h"
#include <d3d11.h>

namespace joj
{
	enum class RenderOptions
	{
		Lighting,
		Textures,
		TexturesAndFog
	};

	enum class RasterizerStateOption
	{
		Wireframe,
		NoCull,
		CullClockwise,
		None
	};

	enum class BlendStateOption
	{
		AlphaToCoverage,
		Transparent,
		NoRenderTargetWrite,
		None
	};

	enum class DepthStencilStateOption
	{
		MarkMirror,
		Reflection,
		NoDoubleBlend,
		LessEqual,
		None
	};

	class JAPI D3D11RenderState
	{
	public:
		D3D11RenderState();
		~D3D11RenderState();

		ErrorCode create_rasterizer_state(RasterizerStateOption rasterizer_state_option);
		ErrorCode create_blend_state(BlendStateOption blend_state_option);
		ErrorCode create_depthstencil_state(DepthStencilStateOption depthstencil_state_option);

		void set_rasterizer_state(RasterizerStateOption rasterizer_state_option);
		void set_blend_state(BlendStateOption blend_state_option, f32 blend_factor[4] = nullptr);
		void set_depthstencil_state(DepthStencilStateOption depthstencil_state_option);

	private:
		ID3D11RasterizerState* m_wireframe_RS;
		ID3D11RasterizerState* m_no_cull_RS;
		ID3D11RasterizerState* m_cull_clock_wise_RS;

		ID3D11BlendState* m_alpha_to_coverage_BS;
		ID3D11BlendState* m_transparent_BS;
		ID3D11BlendState* m_no_render_target_write_BS;

		ID3D11DepthStencilState* m_mark_mirror_DSS;
		ID3D11DepthStencilState* m_draw_reflection_DSS;
		ID3D11DepthStencilState* m_no_double_blend_DSS;
		ID3D11DepthStencilState* m_less_equal_DSS;
	};
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_RENDER_STATE_H