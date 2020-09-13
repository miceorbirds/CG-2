#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "color.h"
#include <assimp/material.h>

enum class TextureStorageType
{
	Invalid,
	None,
	EmbeddedIndexCompressed,
	EmbeddedIndexNonCompressed,
	EmbeddedCompressed,
	EmbeddedNonCompressed,
	Disk
};

class Texture
{
public:
	Texture(ID3D11Device* device, const Color& color, aiTextureType type);
	Texture(ID3D11Device* device, const Color* color_data, UINT width, UINT height, aiTextureType type); //Generate texture of specific color data
	Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type);
	Texture(ID3D11Device* device, const uint8_t* data, size_t size, aiTextureType type);

	aiTextureType GetType();
	ID3D11ShaderResourceView* GetTextureResourceView();
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();

private:
	void Initialize1x1ColorTexture(ID3D11Device* device, const Color& color_data, aiTextureType type);
	void InitializeColorTexture(ID3D11Device* device, const Color* color_data, UINT width, UINT height, aiTextureType type);
	Microsoft::WRL::ComPtr<ID3D11Resource> m_texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture_view = nullptr;
	aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;
};