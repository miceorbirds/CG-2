#include "camera.h"

Camera::Camera()
{
	this->m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateViewMatrix();
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	const auto fov_radians = (fovDegrees / 360.0f) * XM_2PI;
	this->m_projection_matrix = XMMatrixPerspectiveFovLH(fov_radians, aspectRatio, nearZ, farZ);
}

const XMMATRIX& Camera::GetViewMatrix() const
{
	return this->m_view_matrix;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
	return this->m_projection_matrix;
}

const XMVECTOR& Camera::GetPositionVector() const
{
	return this->m_pos_vector;
}

const XMFLOAT3& Camera::GetPositionFloat3() const
{
	return this->m_pos;
}

const XMVECTOR& Camera::GetRotationVector() const
{
	return this->m_rot_vector;
}

const XMFLOAT3& Camera::GetRotationFloat3() const
{
	return this->m_rot;
}

void Camera::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->m_pos, pos);
	this->m_pos_vector = pos;
	this->UpdateViewMatrix();
}

void Camera::SetPosition(float x, float y, float z)
{
	this->m_pos = XMFLOAT3(x, y, z);
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition(const XMVECTOR& pos)
{
	this->m_pos_vector += pos;
	XMStoreFloat3(&this->m_pos, this->m_pos_vector);
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition(float x, float y, float z)
{
	this->m_pos.x += x;
	this->m_pos.y += y;
	this->m_pos.z += z;
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(const XMVECTOR& rot)
{
	this->m_rot_vector = rot;
	XMStoreFloat3(&this->m_rot, rot);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(float x, float y, float z)
{
	this->m_rot = XMFLOAT3(x, y, z);
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation(const XMVECTOR& rot)
{
	this->m_rot_vector += rot;
	XMStoreFloat3(&this->m_rot, this->m_rot_vector);
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation(float x, float y, float z)
{
	this->m_rot.x += x;
	this->m_rot.y += y;
	this->m_rot.z += z;
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateViewMatrix();
}

void Camera::SetLookAtPos(XMFLOAT3 look_at_pos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (look_at_pos.x == this->m_pos.x && look_at_pos.y == this->m_pos.y && look_at_pos.z == this->m_pos.z)
		return;

	look_at_pos.x = this->m_pos.x - look_at_pos.x;
	look_at_pos.y = this->m_pos.y - look_at_pos.y;
	look_at_pos.z = this->m_pos.z - look_at_pos.z;

	auto pitch = 0.0f;
	if (look_at_pos.y != 0.0f)
	{
		const float distance = sqrt(look_at_pos.x * look_at_pos.x + look_at_pos.z * look_at_pos.z);
		pitch = atan(look_at_pos.y / distance);
	}

	auto yaw = 0.0f;
	if (look_at_pos.x != 0.0f)
	{
		yaw = atan(look_at_pos.x / look_at_pos.z);
	}
	if (look_at_pos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& Camera::GetForwardVector() const
{
	return this->m_vec_forward;
}

const XMVECTOR& Camera::GetRightVector() const
{
	return this->m_vec_right;
}

const XMVECTOR& Camera::GetBackwardVector() const
{
	return this->m_vec_backward;
}

const XMVECTOR& Camera::GetLeftVector() const
{
	return this->m_vec_left;
}


void Camera::UpdateViewMatrix() //Updates view matrix and also updates the movement vectors
{
	//Calculate camera rotation matrix
	const auto cam_rotation_matrix = XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z);
	//Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	auto cam_target = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, cam_rotation_matrix);
	//Adjust cam target to be offset by the camera's current position
	cam_target += this->m_pos_vector;
	//Calculate up direction based on current rotation
	const auto up_dir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, cam_rotation_matrix);
	//Rebuild view matrix
	this->m_view_matrix = XMMatrixLookAtLH(this->m_pos_vector, cam_target, up_dir);

	const auto vec_rotation_matrix = XMMatrixRotationRollPitchYaw(0.0f, this->m_rot.y, 0.0f);
	this->m_vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vec_rotation_matrix);
	this->m_vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vec_rotation_matrix);
	this->m_vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vec_rotation_matrix);
	this->m_vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vec_rotation_matrix);

}
