#include "shadow_map.h"

bool ShadowMap::Initialize(ID3D11Device* device)
{
	try
	{
		// create shadowmap texture that we will use as a render target for write in the depth and as a shader resourse for computation of shadows)
		CD3D11_TEXTURE2D_DESC shadowmap_texture_description(DXGI_FORMAT_R24G8_TYPELESS, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
		shadowmap_texture_description.MipLevels = 1;
		shadowmap_texture_description.BindFlags = D3D11_BIND_DEPTH_STENCIL |
			D3D11_BIND_SHADER_RESOURCE;

		auto hr = device->CreateTexture2D(&shadowmap_texture_description, NULL, m_shadowmap_texture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed create depth RenderTarget CreateTexture2D");
		// create depth stencil view using shadowmap texture
		CD3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT);
		hr = device->CreateDepthStencilView(m_shadowmap_texture.Get(), &depth_stencil_view_desc, m_depth_stensil_view.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed create depth stencil view for shadowmap");
		// create shader resource view using shadowmap texture
		CD3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
		shader_resource_view_desc.Texture2D.MipLevels = shadowmap_texture_description.MipLevels;
		hr = device->CreateShaderResourceView(m_shadowmap_texture.Get(), &shader_resource_view_desc, m_shader_resource_view.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed create shader_resource_texture for shadowmap");

		// create viewport with dimensions of our shadowmap
		m_viewport.Width = SHADOWMAP_WIDTH;
		m_viewport.Height = SHADOWMAP_HEIGHT;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
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
	device_context->RSSetViewports(1, &m_viewport);

	//Set null render target because we are only going to draw to depth buffer. Setting a null render target will disable color 	writes.

	ID3D11ShaderResourceView* pNullSRV = NULL;
	device_context->PSSetShaderResources(1, 1, &pNullSRV);
	device_context->OMSetRenderTargets(0, 0, m_depth_stensil_view.Get());
	device_context->ClearDepthStencilView(this->m_depth_stensil_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* ShadowMap::GetShaderResourceView()
{
	return this->m_shader_resource_view.Get();
}

ID3D11ShaderResourceView** ShadowMap::GetShaderResourceViewAddress()
{
	return this->m_shader_resource_view.GetAddressOf();
}