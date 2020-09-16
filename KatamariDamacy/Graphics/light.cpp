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

bool Light::Initialize(ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->isSun = true;
	this->SetPosition(100.0f, 10.0, 0.0);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}