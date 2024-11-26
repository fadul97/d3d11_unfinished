#ifndef joj_D3D11_BLUR_FILTER_H
#define joj_D3D11_BLUR_FILTER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>
#include "error_code.h"

namespace joj
{
	class JAPI D3D11BlurFilter
	{
	public:
		D3D11BlurFilter();
		~D3D11BlurFilter();


		ID3D11ShaderResourceView* get_blurred_output();

		// Generate Gaussian blur weights.
		void set_gaussian_weights(f32 sigma);

		// Manually specify blur weights.
		void set_weights(const f32 weights[9]);

		/// The width and height should match the dimensions of the input texture to blur.
		/// It is OK to call Init() again to reinitialize the blur filter with a different 
		/// dimension or format.
		ErrorCode init(ID3D11Device* device, u32 width, u32 height, DXGI_FORMAT format);

		/// Blurs the input texture blurCount times.  Note that this modifies the input texture, not a copy of it.
		void blur_in_place(ID3D11DeviceContext* dc,
			ID3D11ShaderResourceView* input_SRV,
			ID3D11UnorderedAccessView* input_UAV,
			i32 blur_count);

	private:
		u32 m_width;
		u32 m_height;
		DXGI_FORMAT m_format;

		ID3D11ShaderResourceView* m_blurred_output_tex_SRV;
		ID3D11UnorderedAccessView* m_blurred_output_tex_UAV;
	};
}

#endif // JPLATFORM_WINDOWS

#endif // joj_D3D11_BLUR_FILTER_H