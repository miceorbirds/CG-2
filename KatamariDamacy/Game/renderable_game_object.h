#pragma once
#include "game_object.h"

class RenderableGameObject : public GameObject
{
public:
	bool Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	bool Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool set_texture_manually, const std::string& texture_file_path);
	void Draw(const XMMATRIX& viewProjectionMatrix);
	const XMMATRIX& GetWorldMatrix() const;
	const XMVECTOR GetMaxDirection();
	const XMVECTOR GeMinDirection();
	void SetSize(float size);
	const bool CheckColision(RenderableGameObject& gameObject);
	const bool CheckFutureColision(RenderableGameObject& gameObject, XMVECTOR adjustPosition);
	const bool CanAttach(float curSize);
	//void Rotate(Vector3 rot, float dt);
	void AttachToMain(RenderableGameObject* mainObject);
	bool IsAttachedToMain();
	float GetSize();
	void SetRotation(const XMFLOAT3& rot);
	void SetRotation(float x, float y, float z);
protected:
	Model m_model;
	float rotationSpeed = 0.005f;
	float m_size = 1;
	void UpdateMatrix() override;
	XMMATRIX rotMatrix;
	RenderableGameObject* mainGameObject = nullptr;
	//Vector3 mainObjectR;

	XMMATRIX m_world_matrix = XMMatrixIdentity();
};