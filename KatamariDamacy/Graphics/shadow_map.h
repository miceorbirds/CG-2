#pragma once
#include "vertex.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "const_buffer.h"

class ShadowMap
{
public:
	float SHADOWMAP_WIDTH = 2048;
	float SHADOWMAP_HEIGHT = 2048;

	bool Initialize(ID3D11Device* device);

	void SetShadowmapRenderTarget(ID3D11DeviceContext* device_context);

	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11ShaderResourceView** GetShaderResourceViewAddress();

	//	DirectX::XMMATRIX GetProjectionMatrix();
private:
	//	DirectX::XMMATRIX m_projectionMatrix;
	//	DirectX::XMMATRIX m_orthoMatrix;

	D3D11_VIEWPORT m_viewport;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_shadowmap_texture;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depth_stensil_view;
};