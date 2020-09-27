#include "shadow_map.h"

bool ShadowMap::Initialize(ID3D11Device* device, float screenNear, float screenDepth)
{
	try
	{
		// create shadowmap texture that we will use as a render target for write in the depth and as a shader resourse for computation of shadows)
		CD3D11_TEXTURE2D_DESC shadowmap_texture_description(DXGI_FORMAT_R32G32B32A32_FLOAT, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
		shadowmap_texture_description.MipLevels = 1;
		shadowmap_texture_description.SampleDesc.Count = 1;
		shadowmap_texture_description.BindFlags = D3D11_BIND_RENDER_TARGET |
			D3D11_BIND_SHADER_RESOURCE;
		auto hr = device->CreateTexture2D(&shadowmap_texture_description, nullptr, m_RTTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed create depth RenderTarget CreateTexture2D");

		// create render target view
		D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
		render_target_view_desc.Format = shadowmap_texture_description.Format;
		render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		render_target_view_desc.Texture2D.MipSlice = 0;
		hr = device->CreateRenderTargetView(m_RTTexture.Get(), &render_target_view_desc, m_RTV.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "RenderTarget CreateRenderTargetView");

		// create shader resource view from render target texture
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = shadowmap_texture_description.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(m_RTTexture.Get(), &shaderResourceViewDesc, m_SRV.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "RenderTarget CreateShaderResourceView");

		// create depth stencil texture
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = SHADOWMAP_WIDTH;
		depthBufferDesc.Height = SHADOWMAP_HEIGHT;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&depthBufferDesc, NULL, m_DSTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "RenderTarget CreateTexture2D depth");
		//create depthstensil view
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = depthBufferDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(m_DSTexture.Get(), &depthStencilViewDesc, m_DSV.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "RenderTarget CreateDepthStencilView");

		// create viewport with dimensions of our shadowmap
		m_viewport.Width = SHADOWMAP_WIDTH;
		m_viewport.Height = SHADOWMAP_HEIGHT;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;

		// create projection matrix
		m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(((float)DirectX::XM_PI / 4.0f), (SHADOWMAP_WIDTH / SHADOWMAP_HEIGHT), screenNear, screenDepth);
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void ShadowMap::SetShadowmapRenderTarget(ID3D11DeviceContext* device_context)
{
	device_context->OMSetRenderTargets(1, m_RTV.GetAddressOf(), m_DSV.Get());
	device_context->RSSetViewports(1, &m_viewport);
	//Set null render target because we are only going to draw to depth buffer. Setting a null render target will disable color 	writes.

	//ID3D11ShaderResourceView* pNullSRV = NULL;
	//device_context->PSSetShaderResources(1, 1, &pNullSRV);

	float color[4] = { 0.5, 0.5, 0.5, 1.0 };
	device_context->ClearRenderTargetView(m_RTV.Get(), color);
	device_context->ClearDepthStencilView(this->m_DSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* ShadowMap::GetShaderResourceView()
{
	return this->m_SRV.Get();
}

ID3D11ShaderResourceView** ShadowMap::GetShaderResourceViewAddress()
{
	return this->m_SRV.GetAddressOf();
}

DirectX::XMMATRIX ShadowMap::GetProjectionMatrix()
{
	return m_projectionMatrix;
}