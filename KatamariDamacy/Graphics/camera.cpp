#include "camera.h"

Camera::Camera()
{
	this->m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->m_rot_vector = XMLoadFloat3(&this->m_rot);
	this->UpdateMatrix();
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

void Camera::UpdateMatrix() //Updates view matrix and also updates the movement vectors
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