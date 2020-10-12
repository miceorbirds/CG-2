#include "pointlight.h"

bool PointLightMesh::Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, int pointlightnumber)
{
	if (!m_model.Initialize("Data/Objects/Samples/sphere.obj", device, device_context, cb_vs_vertexshader))
		return false;

	SetPosition(getRandomNumber(-40.0f, 40.0f), getRandomNumber(0.0f, 10.0f), getRandomNumber(-40.0f, 40.0f));
	this->SetSize(10.f);
	this->m_light_radius = 5.f;
	this->_lightColor = XMFLOAT3(getRandomNumber(0.0f, 1.0f), getRandomNumber(0.0f, 1.0f), getRandomNumber(0.0f, 1.0f));
	SetRotation(XMFLOAT3(0, 0, 0));
	this->UpdateMatrix();
	return true;
}

XMFLOAT3 PointLightMesh::GetLightColor()
{
	return _lightColor;
}

float PointLightMesh::GetLightRadius()
{
	return m_light_radius;
}

void PointLightMesh::UpdateMatrix()
{
	m_world_matrix = XMMatrixIdentity();
	this->m_world_matrix = XMMatrixScaling(this->m_size, this->m_size, this->m_size);
	this->m_world_matrix *= XMMatrixTranslation(this->m_pos.x, this->m_pos.y, this->m_pos.z);

	UpdateDirectionVectors();
}
