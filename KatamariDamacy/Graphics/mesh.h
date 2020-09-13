#pragma once
#include "vertex.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "const_buffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "texture.h"

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* device_context, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture> textures, const DirectX::XMMATRIX& transform_matrix);
	Mesh(const Mesh& mesh);
	void Draw();
	const DirectX::XMMATRIX& GetTransformMatrix();
private:
	VertexBuffer<Vertex> m_vertex_buffer;
	IndexBuffer m_index_buffer;
	ID3D11DeviceContext* m_device_context;
	std::vector<Texture> m_textures;
	DirectX::XMMATRIX m_transform_matrix;
};