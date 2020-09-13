#include "game_object.h"

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
	this->UpdateMatrix();
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	this->m_pos = pos;
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateMatrix();
}

void GameObject::SetPosition(float x, float y, float z)
{
	this->m_pos = XMFLOAT3(x, y, z);
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMVECTOR& pos)
{
	this->m_pos_vector += pos;
	XMStoreFloat3(&this->m_pos, this->m_pos_vector);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(const XMFLOAT3& pos)
{
	this->m_pos.x += pos.y;
	this->m_pos.y += pos.y;
	this->m_pos.z += pos.z;
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	this->m_pos.x += x;
	this->m_pos.y += y;
	this->m_pos.z += z;
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateMatrix();
}

void GameObject::SetRotation(const XMVECTOR& rot)
{
	this->m_rot_vector = rot;
	XMStoreFloat3(&this->m_rot, rot);
	this->UpdateMatrix();
}

void GameObject::SetRotation(const XMFLOAT3& rot)
{
	this->m_rot = rot;
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	this->m_rot = XMFLOAT3(x, y, z);
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const XMVECTOR& rot)
{
	this->m_rot_vector += rot;
	XMStoreFloat3(&this->m_rot, this->m_rot_vector);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(const XMFLOAT3& rot)
{
	this->m_rot.x += rot.x;
	this->m_rot.y += rot.y;
	this->m_rot.z += rot.z;
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateMatrix();
}

void GameObject::AdjustRotation(float x, float y, float z)
{
	this->m_rot.x += x;
	this->m_rot.y += y;
	this->m_rot.z += z;
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateMatrix();
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
void GameObject::UpdateMatrix()
{
	assert("UpdateMatrix must be overridden." && 0);
}