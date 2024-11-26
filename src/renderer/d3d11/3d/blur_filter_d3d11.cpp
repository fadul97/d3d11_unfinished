#include "renderer/d3d11/3d/blur_filter_d3d11.h"

#if JPLATFORM_WINDOWS

#include <corecrt_math.h>
#include "logger.h"

joj::D3D11BlurFilter::D3D11BlurFilter()
{
	m_width = 0;
	m_height = 0;
	m_format = DXGI_FORMAT_UNKNOWN;
	m_blurred_output_tex_SRV = nullptr;
	m_blurred_output_tex_UAV = nullptr;
}

joj::D3D11BlurFilter::~D3D11BlurFilter()
{
	// Release resources
	if (m_blurred_output_tex_SRV)
	{
		m_blurred_output_tex_SRV->Release();
		m_blurred_output_tex_SRV = nullptr;
	}

	if (m_blurred_output_tex_UAV)
	{
		m_blurred_output_tex_UAV->Release();
		m_blurred_output_tex_UAV = nullptr;
	}
}

ID3D11ShaderResourceView* joj::D3D11BlurFilter::get_blurred_output()
{
	return m_blurred_output_tex_SRV;
}

void joj::D3D11BlurFilter::set_gaussian_weights(f32 sigma)
{
	f32 d = 2.0f * sigma * sigma;

	f32 weights[9];
	f32 sum = 0.0f;
	for (i32 i = 0; i < 8; ++i)
	{
		f32 x = (f32)i;
		weights[i] = expf(-x * x / d);

		sum += weights[i];
	}

	// Divide by the sum so all the weights add up to 1.0.
	for (i32 i = 0; i < 8; ++i)
	{
		weights[i] /= sum;
	}

	// TODO:
	// Effects::BlurFX->SetWeights(weights);
}

// TODO:
void joj::D3D11BlurFilter::set_weights(const f32 weights[9])
{

}

joj::ErrorCode joj::D3D11BlurFilter::init(ID3D11Device* device, u32 width, u32 height, DXGI_FORMAT format)
{
	// ---------------------------------------------------
	// Release resources
	// ---------------------------------------------------
	if (m_blurred_output_tex_SRV)
	{
		m_blurred_output_tex_SRV->Release();
		m_blurred_output_tex_SRV = nullptr;
	}

	if (m_blurred_output_tex_UAV)
	{
		m_blurred_output_tex_UAV->Release();
		m_blurred_output_tex_UAV = nullptr;
	}

	m_width = width;
	m_height = height;
	m_format = format;

	D3D11_TEXTURE2D_DESC blurred_tex_desc;
	blurred_tex_desc.Width = width;
	blurred_tex_desc.Height = height;
	blurred_tex_desc.MipLevels = 1;
	blurred_tex_desc.ArraySize = 1;
	blurred_tex_desc.Format = format;
	blurred_tex_desc.SampleDesc.Count = 1;
	blurred_tex_desc.SampleDesc.Quality = 0;
	blurred_tex_desc.Usage = D3D11_USAGE_DEFAULT;
	blurred_tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	blurred_tex_desc.CPUAccessFlags = 0;
	blurred_tex_desc.MiscFlags = 0;

	ID3D11Texture2D* blurred_tex = nullptr;
	if (device->CreateTexture2D(&blurred_tex_desc, 0, &blurred_tex) != S_OK)
	{
		JERROR(ErrorCode::ERR_RENDERER_D3D11_TEXTURE2D_CREATION, "Failed to create blurred texture.");
		return ErrorCode::ERR_RENDERER_D3D11_TEXTURE2D_CREATION;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format = format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels = 1;

	if (device->CreateShaderResourceView(blurred_tex, &srv_desc, &m_blurred_output_tex_SRV) != S_OK)
	{
		JERROR(ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION,
			"Failed to create Shader Resource View.");
		return ErrorCode::ERR_RENDERER_D3D11_SHADER_RESOURCE_VIEW_CREATION;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
	uav_desc.Format = format;
	uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uav_desc.Texture2D.MipSlice = 0;

	if (device->CreateUnorderedAccessView(blurred_tex, &uav_desc, &m_blurred_output_tex_UAV) != S_OK)
	{
		JERROR(ErrorCode::ERR_RENDERER_D3D11_UNORDERED_ACCESS_VIEW_CREATION,
			"Failed to create Unordered Access View.");
		return ErrorCode::ERR_RENDERER_D3D11_UNORDERED_ACCESS_VIEW_CREATION;
	}

	if (blurred_tex)
		blurred_tex->Release();

	return ErrorCode::OK;
}

// TODO:
void joj::D3D11BlurFilter::blur_in_place(ID3D11DeviceContext* dc,
	ID3D11ShaderResourceView* input_SRV,
	ID3D11UnorderedAccessView* input_UAV,
	i32 blur_count)
{

}

#endif // JPLATFORM_WINDOWS