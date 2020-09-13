#include "texture.h"
#include "../error_logger.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

Texture::Texture(ID3D11Device* device, const Color& color, aiTextureType type)
{
	this->Initialize1x1ColorTexture(device, color, type);
}

Texture::Texture(ID3D11Device* device, const Color* color_data, UINT width, UINT height, aiTextureType type)
{
	this->InitializeColorTexture(device, color_data, width, height, type);
}

Texture::Texture(ID3D11Device* device, const std::string& file_path, aiTextureType type)
{
	this->type = type;
	if (StringHelper::GetFileExtension(file_path) == ".dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::StringToWide(file_path).c_str(), m_texture.GetAddressOf(), this->m_texture_view.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
		return;
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(file_path).c_str(), m_texture.GetAddressOf(), this->m_texture_view.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
		return;
	}
}

Texture::Texture(ID3D11Device* device, const uint8_t* data, size_t size, aiTextureType type)
{
	this->type = type;
	HRESULT hr = DirectX::CreateWICTextureFromMemory(device, data, size, this->m_texture.GetAddressOf(), this->m_texture_view.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create Texture from memory.");
}

aiTextureType Texture::GetType()
{
	return this->type;
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
	return this->m_texture_view.Get();
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
	return this->m_texture_view.GetAddressOf();
}

void Texture::Initialize1x1ColorTexture(ID3D11Device* device, const Color& color_data, aiTextureType type)
{
	InitializeColorTexture(device, &color_data, 1, 1, type);
}

void Texture::InitializeColorTexture(ID3D11Device* device, const Color* color_data, UINT width, UINT height, aiTextureType type)
{
	this->type = type;
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	ID3D11Texture2D* p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = color_data;
	initialData.SysMemPitch = width * sizeof(Color);
	HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from color data.");
	m_texture = static_cast<ID3D11Texture2D*>(p2DTexture);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
	hr = device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_texture_view.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture generated from color data.");
}