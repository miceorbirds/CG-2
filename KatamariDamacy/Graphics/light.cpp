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
	this->SetPosition(-10.0f, 10.0f, -2.0f);
	this->SetLookAtPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
	this->UpdateMatrix();
	this->GenerateProjectionMatrix(0.5f, 50.f);
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
	this->m_projection_matrix = XMMatrixOrthographicLH(32, 32, screen_near, screen_depth);
}

void Light::UpdateViewMatrix()
{
	XMVECTOR camPos = XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 0.0f);
	XMVECTOR camLookAt = XMVectorSet(m_rot.x, m_rot.y, m_rot.z, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_view_matrix = XMMatrixLookAtLH(camPos, camLookAt, Up);
}