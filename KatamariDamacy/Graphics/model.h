#pragma once

#include "vertex.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "const_buffer.h"

using namespace DirectX;

class Model
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void Draw(const XMMATRIX& view_projection_matrix) const;
	const XMVECTOR& GetPositionVector() const;
	const XMFLOAT3& GetPositionFloat3() const;
	const XMVECTOR& GetRotationVector() const;
	const XMFLOAT3& GetRotationFloat3() const;

	void SetPosition(const XMVECTOR& pos);
	void SetPosition(const XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const XMVECTOR& pos);
	void AdjustPosition(const XMFLOAT3& pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(const XMVECTOR& rot);
	void SetRotation(const XMFLOAT3& rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const XMVECTOR& rot);
	void AdjustRotation(const XMFLOAT3& rot);
	void AdjustRotation(float x, float y, float z);
	void SetLookAtPos(XMFLOAT3 look_at_pos);
	const XMVECTOR& GetForwardVector() const;
	const XMVECTOR& GetRightVector() const;
	const XMVECTOR& GetBackwardVector() const;
	const XMVECTOR& GetLeftVector() const;
private:
	void UpdateWorldMatrix();

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_device_context = nullptr;
	ConstantBuffer<CB_VS_vertexshader>* m_cb_vs_vertexshader = nullptr;
	ID3D11ShaderResourceView* m_texture = nullptr;

	VertexBuffer<Vertex> m_vertex_buffer;
	IndexBuffer m_index_buffer;

	XMMATRIX m_world_matrix = XMMatrixIdentity();

	XMVECTOR m_pos_vector {0};
	XMVECTOR m_rot_vector{ 0};
	XMFLOAT3 m_pos {0,0,0};
	XMFLOAT3 m_rot {0,0,0};

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR m_vec_forward{0};
	XMVECTOR m_vec_left {0};
	XMVECTOR m_vec_right{0};
	XMVECTOR m_vec_backward{0};
};