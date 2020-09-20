#include "model.h"

const XMVECTOR& Model::GetMinDirections()
{
	return XMLoadFloat3(&XMFLOAT3(this->xMinus, this->yMinus, this->zMinus));
}

const XMVECTOR& Model::GetMaxDirections()
{
	return XMLoadFloat3(&XMFLOAT3(this->xPlus, this->yPlus, this->zPlus));
}

bool Model::Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->m_device = device;
	this->m_device_context = device_context;
	this->m_cb_vs_vertexshader = &cb_vs_vertexshader;
	if (!this->LoadModel(file_path))
		return false;
	return true;
}

bool Model::Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool set_texture_manually, const std::string& texture_file_path)
{
	this->m_device = device;
	this->m_device_context = device_context;
	this->m_cb_vs_vertexshader = &cb_vs_vertexshader;
	m_set_texture_manually = true;
	m_texture_file_path = texture_file_path;
	if (!this->LoadModel(file_path))
		return false;
	return true;
}

void Model::Draw(const XMMATRIX& world_matrix, const XMMATRIX& view_projection_matrix)
{
	this->m_device_context->VSSetConstantBuffers(0, 1, this->m_cb_vs_vertexshader->GetAddressOf());
	for (int i = 0; i < m_meshes.size(); i++)
	{
		//Update Constant buffer with WVP Matrix
		this->m_cb_vs_vertexshader->data.WVP_matrix = m_meshes[i].GetTransformMatrix() * world_matrix * view_projection_matrix; //Calculate World-View-Projection Matrix
		this->m_cb_vs_vertexshader->data.world_matrix = m_meshes[i].GetTransformMatrix() * world_matrix; // calculate World
		this->m_cb_vs_vertexshader->ApplyChanges();
		m_meshes[i].Draw();
	}
}

bool Model::LoadModel(const std::string& file_path)
{
	this->directory = StringHelper::GetDirectoryFromPath(file_path);
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(file_path.c_str(),
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (scene == nullptr)
	{
		const char* what = importer.GetErrorString();
		printf(what);
		MessageBoxA(NULL, what, "Error", MB_ICONERROR);
		return false;
	}
	this->xPlus = -(std::numeric_limits<float>::max)();
	this->xMinus = (std::numeric_limits<float>::max)();
	this->yPlus = -(std::numeric_limits<float>::max)();
	this->yMinus = (std::numeric_limits<float>::max)();
	this->zPlus = -(std::numeric_limits<float>::max)();
	this->zMinus = (std::numeric_limits<float>::max)();

	this->ProcessNode(scene->mRootNode, scene, XMMatrixIdentity());
	return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parent_transform_matrix)
{
	XMMATRIX node_transform_matrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parent_transform_matrix;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.emplace_back(this->ProcessMesh(mesh, scene, node_transform_matrix));
	}
	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, node_transform_matrix);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transform_matrix)
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
		XMFLOAT3 vertexPos;
		XMVECTOR vertexPos_v = XMVector3Transform(XMLoadFloat3(&vertex.pos), transform_matrix);
		XMStoreFloat3(&vertexPos, vertexPos_v);

		if (this->xPlus < vertexPos.x)
		{
			this->xPlus = vertexPos.x;
		}

		if (this->xMinus > vertexPos.x)
		{
			this->xMinus = vertexPos.x;
		}

		if (this->yPlus < vertexPos.y)
		{
			this->yPlus = vertexPos.y;
		}

		if (this->yMinus > vertexPos.y)
		{
			this->yMinus = vertexPos.y;
		}

		if (this->zPlus < vertexPos.z)
		{
			this->zPlus = vertexPos.z;
		}

		if (this->zMinus > vertexPos.z)
		{
			this->zMinus = vertexPos.z;
		}

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

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
	std::vector<Texture> textures;
	if (m_set_texture_manually == true)
	{
		Texture own_texture(m_device, m_texture_file_path, aiTextureType::aiTextureType_DIFFUSE);
		textures.emplace_back(own_texture);
	}
	else
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuse_textures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
		textures.insert(textures.end(), diffuse_textures.begin(), diffuse_textures.end());
	}
	return Mesh(this->m_device, this->m_device_context, vertices, indices, textures, transform_matrix);
}

TextureStorageType Model::DetermineTextureStorageType(const aiScene* scene, aiMaterial* material, unsigned int index, aiTextureType texture_type)
{
	if (material->GetTextureCount(texture_type) == 0)
		return TextureStorageType::None;

	aiString path;
	material->GetTexture(texture_type, index, &path);
	std::string texturePath = path.C_Str();
	//Check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (scene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	//Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = scene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	//Lastly check if texture is a filepath by checking for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None; // No texture exists
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType texture_type, const aiScene* scene)
{
	std::vector<Texture> material_textures;
	TextureStorageType storetype = TextureStorageType::Invalid;
	unsigned int texture_count = material->GetTextureCount(texture_type);

	if (texture_count == 0) //If there are no textures
	{
		storetype = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		switch (texture_type)
		{
		case aiTextureType_DIFFUSE:
			material->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack()) //If color = black, just use grey
			{
				material_textures.push_back(Texture(this->m_device, Colors::UnloadedTextureColor, texture_type));
				return material_textures;
			}
			material_textures.push_back(Texture(this->m_device, Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), texture_type));
			return material_textures;
		}
	}
	else
	{
		for (UINT i = 0; i < texture_count; i++)
		{
			aiString path;
			material->GetTexture(texture_type, i, &path);
			TextureStorageType storetype = DetermineTextureStorageType(scene, material, i, texture_type);
			switch (storetype)
			{
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				int index = GetTextureIndex(&path);
				Texture embeddedIndexedTexture(this->m_device,
					reinterpret_cast<uint8_t*>(scene->mTextures[index]->pcData),
					scene->mTextures[index]->mWidth,
					texture_type);
				material_textures.push_back(embeddedIndexedTexture);
				break;
			}
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture* pTexture = scene->GetEmbeddedTexture(path.C_Str());
				Texture embedded_texture(this->m_device,
					reinterpret_cast<uint8_t*>(pTexture->pcData),
					pTexture->mWidth,
					texture_type);
				material_textures.push_back(embedded_texture);
				break;
			}
			case TextureStorageType::Disk:
			{
				std::string filename = this->directory + '\\' + path.C_Str();
				Texture diskTexture(this->m_device, filename, texture_type);
				material_textures.push_back(diskTexture);
				break;
			}
			}
		}
	}

	if (material_textures.size() == 0)
	{
		material_textures.push_back(Texture(this->m_device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
	}
	return material_textures;
}

int Model::GetTextureIndex(aiString* str)
{
	assert(str->length >= 2);
	return atoi(&str->C_Str()[1]);
}