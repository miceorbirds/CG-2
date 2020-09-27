#include "renderable_game_object.h"

bool RenderableGameObject::Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!m_model.Initialize(file_path, device, device_context, cb_vs_vertexshader))
		return false;
	this->center_bound = this->m_model.centerBound;
	this->radius_bound = this->m_model.radiusBound;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->SetSize(1);
	this->UpdateMatrix();
	return true;
}

bool RenderableGameObject::Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool set_texture_manually, const std::string& texture_file_path)
{
	if (!m_model.Initialize(file_path, device, device_context, cb_vs_vertexshader, set_texture_manually, texture_file_path))
		return false;
	this->center_bound = this->m_model.centerBound;
	this->radius_bound = this->m_model.radiusBound;
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->SetSize(1);
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

bool RenderableGameObject::FindCollision(RenderableGameObject* obj)
{
	XMFLOAT3 vector;

	vector.x = this->GetPositionFloat3().x - obj->GetPositionFloat3().x;
	vector.y = this->GetPositionFloat3().y - obj->GetPositionFloat3().y;
	vector.z = this->GetPositionFloat3().z - obj->GetPositionFloat3().z;

	float length = sqrtf(powf(vector.x, 2) + powf(vector.y, 2) + powf(vector.z, 2));

	float RadiusSum = this->radius_bound + obj->radius_bound;
	return length < RadiusSum;
}

void RenderableGameObject::UpdateMatrix()
{
	if (this->parent != nullptr)
	{
		static float oldX = parent->GetRotationFloat3().x;
		static float oldZ = parent->GetRotationFloat3().z;

		float deltaX = parent->GetRotationFloat3().x - oldX;
		float deltaZ = parent->GetRotationFloat3().z - oldZ;

		oldX = parent->GetRotationFloat3().x;
		oldZ = parent->GetRotationFloat3().z;

		old_rotation *= XMMatrixRotationX(deltaX);
		old_rotation *= XMMatrixRotationZ(-deltaZ);

		XMMATRIX scaleMatrix = XMMatrixScaling(this->m_size, this->m_size, this->m_size);
		this->m_world_matrix = scaleMatrix;
		this->m_world_matrix *= XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z);
		this->m_world_matrix *= XMMatrixTranslation(this->m_pos.x, this->m_pos.y, this->m_pos.z);
		this->m_world_matrix *= old_rotation;
		this->m_world_matrix *= XMMatrixTranslation(parent->GetPositionFloat3().x, parent->GetPositionFloat3().y, parent->GetPositionFloat3().z);

		//if (parent->parent != nullptr)
		//{
		//	this->m_world_matrix *= XMMatrixTranslation(parent->parent->GetPositionFloat3().x, parent->parent->GetPositionFloat3().y, parent->parent->GetPositionFloat3().z);
		//	this->m_world_matrix *= XMMatrixRotationRollPitchYaw(parent->parent->GetRotationRelativeFloat3().x, parent->parent->GetRotationRelativeFloat3().y, parent->parent->GetRotationRelativeFloat3().z);
		//}
	}
	else
	{
		this->m_world_matrix = XMMatrixScaling(this->m_size, this->m_size, this->m_size);
		this->m_world_matrix *= XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z);
		this->m_world_matrix *= XMMatrixTranslation(this->m_pos.x, this->m_pos.y, this->m_pos.z);
	}
	CalcCenterBound();
	UpdateDirectionVectors();
}

const XMFLOAT3& RenderableGameObject::GetRotationRelativeFloat3() const
{
	return this->rot_relative;
}

void RenderableGameObject::CalcCenterBound()
{
	FXMVECTOR vector = XMLoadFloat3(&this->center_bound);
	XMVECTOR NewCenter = XMVector3Transform(vector, m_world_matrix);
	XMStoreFloat3(&this->center_bound, NewCenter);
}

void RenderableGameObject::SetParent(RenderableGameObject* parent)
{
	this->parent = parent;
}

void RenderableGameObject::AddChild(RenderableGameObject* obj)
{
	obj->SetParent(this);
	XMFLOAT3 vector;
	vector.x = obj->GetPositionFloat3().x - this->GetPositionFloat3().x;
	vector.y = obj->GetPositionFloat3().y - this->GetPositionFloat3().y;
	vector.z = obj->GetPositionFloat3().z - this->GetPositionFloat3().z;

	XMVECTOR a = XMLoadFloat3(&vector);
	XMVECTOR Length = XMVector3Length(a);
	XMFLOAT3 vectorLength;
	XMStoreFloat3(&vectorLength, Length);
	float InverseLength = 1 / vectorLength.x;

	vector.x *= InverseLength * (this->radius_bound + this->radius_bound);
	vector.y *= InverseLength * (this->radius_bound + this->radius_bound);
	vector.z *= InverseLength * (this->radius_bound + this->radius_bound);

	obj->SetPosition(vector.x, vector.y, vector.z);
	this->childs.push_back(obj);
	obj->isChild = true;
	obj->UpdateMatrix();
}