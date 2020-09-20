#pragma once
#include "game_object.h"

class RenderableGameObject : public GameObject
{
public:
	bool Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	bool Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool set_texture_manually, const std::string& texture_file_path);
	void Draw(const XMMATRIX& viewProjectionMatrix);
	const XMMATRIX GetWorld();
protected:
	Model m_model;
	void UpdateMatrix() override;

	XMMATRIX m_world_matrix = XMMatrixIdentity();
};