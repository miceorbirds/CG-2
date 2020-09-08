#include "model.h"

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->m_device = device;
	this->m_device_context = device_context;
	this->m_texture = texture;
	this->m_cb_vs_vertexshader = &cb_vs_vertexshader;

	try
	{
		//Textured Square
		Vertex v[] =
		{
			Vertex(-0.5f,  -0.5f, -0.5f, 0.0f, 1.0f), //FRONT Bottom Left   - [0]
			Vertex(-0.5f,   0.5f, -0.5f, 0.0f, 0.0f), //FRONT Top Left      - [1]
			Vertex(0.5f,   0.5f, -0.5f, 1.0f, 0.0f), //FRONT Top Right     - [2]
			Vertex(0.5f,  -0.5f, -0.5f, 1.0f, 1.0f), //FRONT Bottom Right   - [3]
			Vertex(-0.5f,  -0.5f, 0.5f, 0.0f, 1.0f), //BACK Bottom Left   - [4]
			Vertex(-0.5f,   0.5f, 0.5f, 0.0f, 0.0f), //BACK Top Left      - [5]
			Vertex(0.5f,   0.5f, 0.5f, 1.0f, 0.0f), //BACK Top Right     - [6]
			Vertex(0.5f,  -0.5f, 0.5f, 1.0f, 1.0f), //BACK Bottom Right   - [7]
		};

		//Load Vertex Data
		auto hr = this->m_vertex_buffer.Initialize(this->m_device, v, ARRAYSIZE(v));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer.");

		DWORD indices[] =
		{
			0, 1, 2, //FRONT
			0, 2, 3, //FRONT
			4, 7, 6, //BACK 
			4, 6, 5, //BACK
			3, 2, 6, //RIGHT SIDE
			3, 6, 7, //RIGHT SIDE
			4, 5, 1, //LEFT SIDE
			4, 1, 0, //LEFT SIDE
			1, 5, 6, //TOP
			1, 6, 2, //TOP
			0, 3, 7, //BOTTOM
			0, 7, 4, //BOTTOM
		};

		//Load Index Data
		hr = this->m_index_buffer.Initialize(this->m_device, indices, ARRAYSIZE(indices));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateWorldMatrix();
	return true;
}

void Model::SetTexture(ID3D11ShaderResourceView* texture)
{
	this->m_texture = texture;
}

void Model::Draw(const XMMATRIX& view_projection_matrix) const
{
	//Update Constant buffer with WVP Matrix
	this->m_cb_vs_vertexshader->data.mat = this->m_world_matrix * view_projection_matrix; //Calculate World-View-Projection Matrix
	this->m_cb_vs_vertexshader->data.mat = XMMatrixTranspose(this->m_cb_vs_vertexshader->data.mat);
	this->m_cb_vs_vertexshader->ApplyChanges();
	this->m_device_context->VSSetConstantBuffers(0, 1, this->m_cb_vs_vertexshader->GetAddressOf());

	this->m_device_context->PSSetShaderResources(0, 1, &this->m_texture); //Set Texture
	this->m_device_context->IASetIndexBuffer(this->m_index_buffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	UINT offset = 0;
	this->m_device_context->IASetVertexBuffers(0, 1, this->m_vertex_buffer.GetAddressOf(), this->m_vertex_buffer.StridePtr(), &offset);
	this->m_device_context->DrawIndexed(this->m_index_buffer.BufferSize(), 0, 0); //Draw
}

void Model::UpdateWorldMatrix()
{
	this->m_world_matrix = XMMatrixRotationRollPitchYaw(this->m_rot.x, this->m_rot.y, this->m_rot.z) * XMMatrixTranslation(this->m_pos.x, this->m_pos.y, this->m_pos.z);
	const auto vec_rotation_matrix = XMMatrixRotationRollPitchYaw(0.0f, this->m_rot.y, 0.0f);
	this->m_vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vec_rotation_matrix);
	this->m_vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vec_rotation_matrix);
	this->m_vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vec_rotation_matrix);
	this->m_vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vec_rotation_matrix);
}

const XMVECTOR& Model::GetPositionVector() const
{
	return this->m_pos_vector;
}

const XMFLOAT3& Model::GetPositionFloat3() const
{
	return this->m_pos;
}

const XMVECTOR& Model::GetRotationVector() const
{
	return this->m_rot_vector;
}

const XMFLOAT3& Model::GetRotationFloat3() const
{
	return this->m_rot;
}

void Model::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->m_pos, pos);
	this->m_pos_vector = pos;
	this->UpdateWorldMatrix();
}

void Model::SetPosition(const XMFLOAT3& pos)
{
	this->m_pos = pos;
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void Model::SetPosition(float x, float y, float z)
{
	this->m_pos = XMFLOAT3(x, y, z);
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(const XMVECTOR& pos)
{
	this->m_pos_vector += pos;
	XMStoreFloat3(&this->m_pos, this->m_pos_vector);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(const XMFLOAT3& pos)
{
	this->m_pos.x += pos.y;
	this->m_pos.y += pos.y;
	this->m_pos.z += pos.z;
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void Model::AdjustPosition(float x, float y, float z)
{
	this->m_pos.x += x;
	this->m_pos.y += y;
	this->m_pos.z += z;
	this->m_pos_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(const XMVECTOR& rot)
{
	this->m_rot_vector = rot;
	XMStoreFloat3(&this->m_pos, rot);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(const XMFLOAT3& rot)
{
	this->m_pos = rot;
	this->m_rot_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void Model::SetRotation(float x, float y, float z)
{
	this->m_pos = XMFLOAT3(x, y, z);
	this->m_rot_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(const XMVECTOR& rot)
{
	this->m_rot_vector += rot;
	XMStoreFloat3(&this->m_pos, this->m_rot_vector);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(const XMFLOAT3& rot)
{
	this->m_pos.x += rot.x;
	this->m_pos.y += rot.y;
	this->m_pos.z += rot.z;
	this->m_rot_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void Model::AdjustRotation(float x, float y, float z)
{
	this->m_pos.x += x;
	this->m_pos.y += y;
	this->m_pos.z += z;
	this->m_rot_vector = XMLoadFloat3(&this->m_pos);
	this->UpdateWorldMatrix();
}

void Model::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == this->m_pos.x && lookAtPos.y == this->m_pos.y && lookAtPos.z == this->m_pos.z)
		return;

	lookAtPos.x = this->m_pos.x - lookAtPos.x;
	lookAtPos.y = this->m_pos.y - lookAtPos.y;
	lookAtPos.z = this->m_pos.z - lookAtPos.z;

	auto pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& Model::GetForwardVector() const
{
	return this->m_vec_forward;
}

const XMVECTOR& Model::GetRightVector() const
{
	return this->m_vec_right;
}

const XMVECTOR& Model::GetBackwardVector() const
{
	return this->m_vec_backward;
}

const XMVECTOR& Model::GetLeftVector() const
{
	return this->m_vec_left;
}
