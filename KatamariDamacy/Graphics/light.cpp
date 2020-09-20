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
	this->SetPosition(-2.0f, 4.0f, -1.0f);
	this->SetLookAtPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
	this->UpdateMatrix();
	this->GenerateProjectionMatrix(screen_near, screen_depth);
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
	this->m_projection_matrix = XMMatrixOrthographicLH(10, 10, screen_near, screen_depth);
}

void Light::UpdateViewMatrix()
{
	XMVECTOR light_position = XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 0.0f);
	XMVECTOR light_look_at = XMVectorSet(m_rot.x, m_rot.y, m_rot.z, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_view_matrix = XMMatrixLookAtLH(light_position, light_look_at, up);
}