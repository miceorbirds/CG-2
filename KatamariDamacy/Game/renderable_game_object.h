#pragma once
#include "game_object.h"

class RenderableGameObject : public GameObject
{
public:
	bool Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	bool Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool set_texture_manually, const std::string& texture_file_path);
	void Draw(const XMMATRIX& viewProjectionMatrix);
	const XMMATRIX GetWorld();
	bool FindCollision(RenderableGameObject* obj);
	void SetParent(RenderableGameObject* parent);
	void AddChild(RenderableGameObject* obj);
	bool isChild = false;
	std::vector <RenderableGameObject*> childs;
	RenderableGameObject* parent = nullptr;
	void UpdateMatrix() override;
protected:
	void CalcCenterBound();
	XMFLOAT3 center_bound;
	float radius_bound;
	Model m_model;

	const XMFLOAT3& GetRotationRelativeFloat3() const;
	XMMATRIX m_world_matrix = XMMatrixIdentity();

	XMMATRIX old_rotation = XMMatrixIdentity();
	bool setOldRotate = false;
};