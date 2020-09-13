#pragma once
#include "game_object.h"

class RenderableGameObject : public GameObject
{
public:
	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void Draw(const XMMATRIX& viewProjectionMatrix);

protected:
	Model m_model;
	void UpdateMatrix() override;

	XMMATRIX m_world_matrix = XMMatrixIdentity();
};