#pragma once

#include "mesh.h"

using namespace DirectX;

class Model
{
public:
	const XMVECTOR& GetMinDirections();
	const XMVECTOR& GetMaxDirections();

	bool Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	bool Initialize(const std::string& file_path, ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool set_texture_manually, const std::string& texture_file_path);
	void Draw(const XMMATRIX& world_matrix, const XMMATRIX& view_projection_matrix);

	float xPlus;
	float yPlus;
	float zPlus;
	float xMinus;
	float yMinus;
	float zMinus;
private:
	bool LoadModel(const std::string& file_path);
	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parent_transform_matrix);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transform_matrix);
	TextureStorageType DetermineTextureStorageType(const aiScene* scene, aiMaterial* material, unsigned int index, aiTextureType texture_type);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType texture_type, const aiScene* scene);
	int GetTextureIndex(aiString* str);

	std::vector<Mesh> m_meshes;

	bool m_set_texture_manually{ false };
	std::string m_texture_file_path;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_device_context = nullptr;
	ConstantBuffer<CB_VS_vertexshader>* m_cb_vs_vertexshader = nullptr;

	std::string directory = "";
};