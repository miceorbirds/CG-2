#pragma once

#include "../error_logger.h"
#include "../Game/game_object.h"

constexpr UINT BUFFER_COUNT = 3;

struct GbuffRenderTarget
{
	ID3D11Texture2D* texture = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
};

class GBufferRT
{
public:
	bool Init(ID3D11Device* device, float texture_width, float texture_height);

	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);

	// Получаем текстуру RT в виде shader resource view
	ID3D11ShaderResourceView* GetShaderResourceViewNormal();
	ID3D11ShaderResourceView** GetShaderResourceViewAddressNormal();
	ID3D11ShaderResourceView* GetShaderResourceViewDepth();
	ID3D11ShaderResourceView** GetShaderResourceViewAddressDepth();
	ID3D11ShaderResourceView* GetShaderResourceViewAmbient();
	ID3D11ShaderResourceView** GetShaderResourceViewAddressAmbient();
	ID3D11ShaderResourceView* GetShaderResourceViewTexture();
	ID3D11ShaderResourceView** GetShaderResourceViewAddressTexture();

	XMMATRIX GetProjectionMatrix();

	int SHADOWMAP_WIDTH = 2048;
	int SHADOWMAP_HEIGHT = 2048;

	GbuffRenderTarget m_render_targets[BUFFER_COUNT];

private:
	bool InitNormals(ID3D11Device*);
	bool InitDepth(ID3D11Device*);
	bool InitAmbient(ID3D11Device*);
	bool InitTexture(ID3D11Device*);
	bool InitProjMatrix(float, float);
	XMMATRIX m_projectionMatrix;

	float m_texture_width;
	float m_texture_height;
};