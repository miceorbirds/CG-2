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
	static float lightAngle = 270.0f;
	float radians;
	static float lightPosX = 9.0f;

	radians = lightAngle * 0.0174532925f;
	this->SetDirection(sinf(radians), cosf(radians), 0.0f);
	this->SetPosition(lightPosX, 8.0f, -0.1f);
	this->SetLookAtPos(XMFLOAT3(-lightPosX, 0.0f, 0.0f));
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
	this->m_projection_matrix = XMMatrixOrthographicLH(20.0f, 20.0f, screen_near, screen_depth);
}

void Light::UpdateViewMatrix()
{
	XMVECTOR camPos = XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 0.0f);
	XMVECTOR camLookAt = XMVectorSet(m_rot.x, m_rot.y, m_rot.z, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_view_matrix = XMMatrixLookAtLH(camPos, camLookAt, Up);
}

void Light::SetDirection(float x, float y, float z)
{
	m_direction = XMLoadFloat3(&XMFLOAT3(x, y, z));
	return;
}

XMFLOAT3 Light::GetDirection()
{
	XMFLOAT3 direction;
	XMStoreFloat3(&direction, m_direction);
	return direction;
}