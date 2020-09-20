#include "renderable_game_object.h"

bool RenderableGameObject::Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!m_model.Initialize(file_path, device, device_context, cb_vs_vertexshader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}

bool RenderableGameObject::Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool set_texture_manually, const std::string& texture_file_path)
{
	if (!m_model.Initialize(file_path, device, device_context, cb_vs_vertexshader, set_texture_manually, texture_file_path))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->rotMatrix = XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z);
	this->UpdateMatrix();
	return true;
}

void RenderableGameObject::Draw(const XMMATRIX& viewProjectionMatrix)
{
	if (this->IsAttachedToMain())
	{
		this->UpdateMatrix();
	}
	m_model.Draw(this->m_world_matrix, viewProjectionMatrix);
}

const XMMATRIX& RenderableGameObject::GetWorldMatrix() const
{
	return this->m_world_matrix;
}

void RenderableGameObject::UpdateMatrix()
{
	using namespace DirectX;
	if (!this->IsAttachedToMain())
	{
		this->m_world_matrix = XMMatrixScalingFromVector(XMLoadFloat3(&this->m_scale) * m_size) * rotMatrix;
		this->m_world_matrix = XMMatrixTranslation(this->m_pos.x, this->m_pos.y, this->m_pos.z);
	}
	else
	{
		auto transToWorld = XMMatrixIdentity();
		transToWorld = XMMatrixTranslationFromVector(this->mainGameObject->GetPosition());
		auto transToLocal = XMMatrixIdentity();
		transToLocal = XMMatrixTranslationFromVector(this->GetPosition());

		this->m_world_matrix = XMMatrixIdentity() * XMMatrixScalingFromVector(XMLoadFloat3(&this->m_scale) * m_size) * transToLocal * this->mainGameObject->rotMatrix * transToWorld;
	}
	this->UpdateDirectionVectors();
}

void RenderableGameObject::AttachToMain(RenderableGameObject* mainObject)
{
	auto matrix = XMMatrixInverse(nullptr, mainObject->GetWorldMatrix());
	auto vector = XMVector3Transform(this->m_pos_vector, matrix);
	this->m_pos_vector = vector;
	this->mainGameObject = mainObject;
}

bool RenderableGameObject::IsAttachedToMain()
{
	return this->mainGameObject != nullptr;
}

float RenderableGameObject::GetSize()
{
	return this->m_size;
}

void RenderableGameObject::SetRotation(const XMFLOAT3& rot)
{
	this->rotMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	this->UpdateMatrix();
}

void RenderableGameObject::SetRotation(float x, float y, float z)
{
	this->rotMatrix = XMMatrixRotationRollPitchYaw(x, y, z);
	this->UpdateMatrix();
}

const XMVECTOR RenderableGameObject::GetMaxDirection()
{
	XMVECTOR result = this->m_model.GetMaxDirections() * XMLoadFloat3(&this->m_scale) * this->m_size + XMLoadFloat3(&this->m_pos);
	return result;
}

const XMVECTOR RenderableGameObject::GeMinDirection()
{
	XMVECTOR result = this->m_model.GetMinDirections() * XMLoadFloat3(&this->m_scale) * this->m_size + XMLoadFloat3(&this->m_pos);
	return result;
}

void RenderableGameObject::SetSize(float size)
{
	this->m_size = size;
	this->UpdateMatrix();
}

const bool RenderableGameObject::CheckColision(RenderableGameObject& gameObject)
{
	auto maxDirOwn = this->GetMaxDirection();
	auto minDirOwn = this->GeMinDirection();
	auto maxDirTo = gameObject.GetMaxDirection();
	auto minDirTo = gameObject.GeMinDirection();
	XMFLOAT3 maxDirOwn_v, minDirOwn_v, maxDirTo_v, minDirTo_v;
	XMStoreFloat3(&maxDirOwn_v, maxDirOwn);
	XMStoreFloat3(&minDirOwn_v, minDirOwn);
	XMStoreFloat3(&maxDirTo_v, maxDirTo);
	XMStoreFloat3(&minDirTo_v, minDirTo);
	if (maxDirTo_v.x >= minDirOwn_v.x
		&& maxDirTo_v.z >= minDirOwn_v.z
		&& minDirTo_v.x <= maxDirOwn_v.x
		&& minDirTo_v.z <= maxDirOwn_v.z)
	{
		return true;
	};
	return false;
}

const bool RenderableGameObject::CheckFutureColision(RenderableGameObject& gameObject, XMVECTOR adjustPosition)
{
	auto maxDirOwn = this->GetMaxDirection();
	auto minDirOwn = this->GeMinDirection();
	auto maxDirTo = gameObject.GetMaxDirection() + adjustPosition;
	auto minDirTo = gameObject.GeMinDirection() + adjustPosition;
	XMFLOAT3 maxDirOwn_v, minDirOwn_v, maxDirTo_v, minDirTo_v;
	XMStoreFloat3(&maxDirOwn_v, maxDirOwn);
	XMStoreFloat3(&minDirOwn_v, minDirOwn);
	XMStoreFloat3(&maxDirTo_v, maxDirTo);
	XMStoreFloat3(&minDirTo_v, minDirTo);
	if (maxDirTo_v.x >= minDirOwn_v.x
		&& maxDirTo_v.z >= minDirOwn_v.z
		&& minDirTo_v.x <= maxDirOwn_v.x
		&& minDirTo_v.z <= maxDirOwn_v.z)
	{
		return true;
	};
	return false;
}

const bool RenderableGameObject::CanAttach(float curSize)
{
	return curSize >= this->m_size;
}

//void RenderableGameObject::Rotate(Vector3 rot, float dt)
//{
//	rotMatrix *= Matrix::CreateFromAxisAngle(rot, rotationSpeed * dt);
//	Vector3 scale(0, 0, 0);
//	Quaternion rotQ(0, 0, 0, 0);
//	Vector3 trans(0, 0, 0);
//	rotMatrix.Decompose(scale, rotQ, trans);
//	this->rot = Vector3(rotQ.x, rotQ.y, rotQ.z);
//	this->UpdateMatrix();
//}