#include "pointlight.h"

bool PointLightMesh::Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!m_model.Initialize("Data/Objects/Samples/sphere.obj", device, device_context, cb_vs_vertexshader))
		return false;

	SetPosition(getRandomNumber(-50, 50), 0.f, getRandomNumber(-50, 50));
	this->SetSize(getRandomNumber(0.1f, 5.0f));
	this->m_light_radius = this->m_size * 2;
	this->_lightColor = XMFLOAT3(1.0f, 1.0f, 0.0f);
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
	this->m_world_matrix = XMMatrixScaling(this->m_size, this->m_size, this->m_size);
	this->m_world_matrix *= XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z);
	this->m_world_matrix *= XMMatrixTranslation(this->m_pos.x, this->m_pos.y, this->m_pos.z);

	UpdateDirectionVectors();
}
