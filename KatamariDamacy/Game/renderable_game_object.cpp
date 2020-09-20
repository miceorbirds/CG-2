#include "renderable_game_object.h"

bool RenderableGameObject::Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!m_model.Initialize(file_path, device, device_context, cb_vs_vertexshader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->SetScale(1.0f, 1.0f, 1.0f);
	this->UpdateMatrix();
	return true;
}

bool RenderableGameObject::Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool set_texture_manually, const std::string& texture_file_path)
{
	if (!m_model.Initialize(file_path, device, device_context, cb_vs_vertexshader, set_texture_manually, texture_file_path))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->SetScale(0.1f, 0.1f, 0.1f);
	this->UpdateMatrix();
	return true;
}

void RenderableGameObject::Draw(const XMMATRIX& viewProjectionMatrix)
{
	m_model.Draw(this->m_world_matrix, viewProjectionMatrix);
}

const XMMATRIX RenderableGameObject::GetWorld()
{
	this->UpdateMatrix();
	return m_world_matrix;
}

void RenderableGameObject::UpdateMatrix()
{
	this->m_world_matrix = XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z) * XMMatrixTranslation(this->m_pos.x, this->m_pos.y, this->m_pos.z) * XMMatrixScaling(this->m_scale.x, this->m_scale.y, this->m_scale.z);
	this->UpdateDirectionVectors();
}