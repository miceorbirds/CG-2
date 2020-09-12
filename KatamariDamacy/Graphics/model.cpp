#include "model.h"

bool Model::Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->m_device = device;
	this->m_device_context = device_context;
	this->m_texture = texture;
	this->m_cb_vs_vertexshader = &cb_vs_vertexshader;

	try
	{
		if (!this->LoadModel(file_path))
			return false;
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

bool Model::LoadModel(const std::string& file_path)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(file_path,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (pScene == nullptr)
		return false;
	this->ProcessNode(pScene->mRootNode, pScene);
	return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->ProcessMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;

	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return Mesh(this->m_device, this->m_device_context, vertices, indices);
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

void Model::SetLookAtPos(XMFLOAT3 look_at_pos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (look_at_pos.x == this->m_pos.x && look_at_pos.y == this->m_pos.y && look_at_pos.z == this->m_pos.z)
		return;

	look_at_pos.x = this->m_pos.x - look_at_pos.x;
	look_at_pos.y = this->m_pos.y - look_at_pos.y;
	look_at_pos.z = this->m_pos.z - look_at_pos.z;

	auto pitch = 0.0f;
	if (look_at_pos.y != 0.0f)
	{
		const float distance = sqrt(look_at_pos.x * look_at_pos.x + look_at_pos.z * look_at_pos.z);
		pitch = atan(look_at_pos.y / distance);
	}

	auto yaw = 0.0f;
	if (look_at_pos.x != 0.0f)
	{
		yaw = atan(look_at_pos.x / look_at_pos.z);
	}
	if (look_at_pos.z > 0)
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
