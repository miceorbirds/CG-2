#pragma once
#include "../Game/game_object.h"

struct TextureRenderTarget
{
	ID3D11Texture2D* texture = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
};

class GBufferRT
{
public:
	GBufferRT();

	bool Init(ID3D11Device*, float, float);

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

private:
	bool InitNormals(ID3D11Device*);
	bool InitDepth(ID3D11Device*);
	bool InitAmbient(ID3D11Device*);
	bool InitTexture(ID3D11Device*);
	bool InitProjMatrix(float, float);
	XMMATRIX m_projectionMatrix;

	//Normals
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_RTTexture_Normal;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DSTexture_Normal;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV_Normal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV_Normal;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV_Normal;
	D3D11_VIEWPORT m_viewport_Normal;

	//Depth
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_RTTexture_Depth;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DSTexture_Depth;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV_Depth;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV_Depth;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV_Depth;
	D3D11_VIEWPORT m_viewport_Depth;

	//Ambient
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_RTTexture_Ambient;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DSTexture_Ambient;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV_Ambient;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV_Ambient;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV_Ambient;
	D3D11_VIEWPORT m_viewport_Ambient;

	//Texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_RTTexture_Texture;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DSTexture_Texture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV_Texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV_Texture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV_Texture;
	D3D11_VIEWPORT m_viewport_Texture;
};