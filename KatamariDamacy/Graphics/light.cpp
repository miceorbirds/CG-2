#include "Light.h"

bool Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!m_model.Initialize("Data/Objects/light.fbx", device, device_context, cb_vs_vertexshader))
		return false;
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}

bool Light::Initialize(float screen_near, float screen_depth)
{
	this->isSun = true;
	this->SetPosition(-5.0f, 8.0f, -5.0f);
	this->SetLookAtPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
	this->UpdateMatrix();
	this->GenerateProjectionMatrix(1.0f, 100.f);
	this->UpdateViewMatrix();
	this->UpdateMatrix();
	return true;
}

XMMATRIX Light::GetViewMatrix()
{
	return this->m_view_matrix;
}

XMMATRIX Light::GetProjectionMatrix()
{
	return this->m_projection_matrix;
}

void Light::GenerateProjectionMatrix(float screen_near, float screen_depth)
{
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;
	this->m_projection_matrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screen_near, screen_depth);
}

void Light::UpdateViewMatrix()
{
	XMVECTOR camPos = XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 0.0f);
	XMVECTOR camLookAt = XMVectorSet(m_rot.x, m_rot.y, m_rot.z, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_view_matrix = XMMatrixLookAtLH(camPos, camLookAt, Up);
	////Calculate camera rotation matrix
	//const auto cam_rotation_matrix = XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z);
	////Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	//auto cam_target = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, cam_rotation_matrix);
	////Calculate up direction based on current rotation
	//const auto up_dir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, cam_rotation_matrix);
	//XMVECTOR shadow_camera_position = this->GetPositionVector();

	//this->m_view_matrix = XMMatrixLookAtLH(shadow_camera_position, cam_target, up_dir);
}