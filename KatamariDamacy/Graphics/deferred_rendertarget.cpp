#include "deferred_rendertarget.h"

GBufferRT::GBufferRT()
{
	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		m_renderTargetTextureArray[i] = 0;
		m_renderTargetViewArray[i] = 0;
		m_shaderResourceViewArray[i] = 0;
	}
}

bool GBufferRT::Init(ID3D11Device* device, float texture_width, float texture_height)
{
	try
	{
		this->m_texture_height = texture_height;
		this->m_texture_width = texture_width;

		//Generate the render target textures (except depth i think).
		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Width = texture_width;
		textureDesc.Height = texture_height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		HRESULT hr;
		for (UINT i = 0; i < BUFFER_COUNT; i++)
			hr = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTextureArray[i]);
		COM_ERROR_IF_FAILED(hr, "Failed to create texture description for Render targets in gbuffer.");

		//Generate the render target views.
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		for (UINT i = 0; i < BUFFER_COUNT; ++i)
			hr = device->CreateRenderTargetView(m_renderTargetTextureArray[i], &renderTargetViewDesc, &m_renderTargetViewArray[i]);
		COM_ERROR_IF_FAILED(hr, "Failed to create render target views from textures in gbuffer.");

		//Generate the shader resource views.
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		for (UINT i = 0; i < BUFFER_COUNT; ++i)
			hr = device->CreateShaderResourceView(m_renderTargetTextureArray[i], &shaderResourceViewDesc, &m_shaderResourceViewArray[i]);
		COM_ERROR_IF_FAILED(hr, "Failed to create render shader resource views from textures in gbuffer.");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}