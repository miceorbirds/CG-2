#include "game_object.h"

bool GameObject::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!m_model.Initialize(filePath, device, deviceContext, texture, cb_vs_vertexshader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateWorldMatrix();
	return true;
}

void GameObject::Draw(const XMMATRIX& viewProjectionMatrix)
{
	m_model.Draw(this->m_world_matrix, viewProjectionMatrix);
}

void GameObject::UpdateWorldMatrix()
{
	this->m_world_matrix = XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z) * XMMatrixTranslation(this->m_pos.x, this->m_pos.y, this->m_pos.z);
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->m_rot.y, 0.0f);
	this->m_vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->m_vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->m_vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->m_vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}

const XMVECTOR& GameObject::GetPositionVector() const
{
	return this->m_pos_vector;
}

const XMFLOAT3& GameObject::GetPositionFloat3() const
{
	return this->m_pos;
}

const XMVECTOR& GameObject::GetRotationVector() const
{
	return this->m_rot_vector;
}

const XMFLOAT3& GameObject::GetRotationFloat3() const
{
	return this->m_rot;
}

void GameObject::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->m_pos, pos);
	this->m_pos_vector = pos;
	this->UpdateWorldMatrix();
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	this->m_pos = pos;
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void GameObject::SetPosition(float x, float y, float z)
{
	this->m_pos = XMFLOAT3(x, y, z);
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void GameObject::AdjustPosition(const XMVECTOR& pos)
{
	this->m_pos_vector += pos;
	XMStoreFloat3(&this->m_pos, this->m_pos_vector);
	this->UpdateWorldMatrix();
}

void GameObject::AdjustPosition(const XMFLOAT3& pos)
{
	this->m_pos.x += pos.y;
	this->m_pos.y += pos.y;
	this->m_pos.z += pos.z;
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	this->m_pos.x += x;
	this->m_pos.y += y;
	this->m_pos.z += z;
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void GameObject::SetRotation(const XMVECTOR& rot)
{
	this->m_rot_vector = rot;
	XMStoreFloat3(&this->m_rot, rot);
	this->UpdateWorldMatrix();
}

void GameObject::SetRotation(const XMFLOAT3& rot)
{
	this->m_rot = rot;
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateWorldMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	this->m_rot = XMFLOAT3(x, y, z);
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateWorldMatrix();
}

void GameObject::AdjustRotation(const XMVECTOR& rot)
{
	this->m_rot_vector += rot;
	XMStoreFloat3(&this->m_rot, this->m_rot_vector);
	this->UpdateWorldMatrix();
}

void GameObject::AdjustRotation(const XMFLOAT3& rot)
{
	this->m_rot.x += rot.x;
	this->m_rot.y += rot.y;
	this->m_rot.z += rot.z;
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateWorldMatrix();
}

void GameObject::AdjustRotation(float x, float y, float z)
{
	this->m_rot.x += x;
	this->m_rot.y += y;
	this->m_rot.z += z;
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateWorldMatrix();
}

void GameObject::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == this->m_pos.x && lookAtPos.y == this->m_pos.y && lookAtPos.z == this->m_pos.z)
		return;

	lookAtPos.x = this->m_pos.x - lookAtPos.x;
	lookAtPos.y = this->m_pos.y - lookAtPos.y;
	lookAtPos.z = this->m_pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& GameObject::GetForwardVector()
{
	return this->m_vec_forward;
}

const XMVECTOR& GameObject::GetRightVector()
{
	return this->m_vec_right;
}

const XMVECTOR& GameObject::GetBackwardVector()
{
	return this->m_vec_backward;
}

const XMVECTOR& GameObject::GetLeftVector()
{
	return this->m_vec_left;
}