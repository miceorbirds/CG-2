#pragma once
#include "vertex.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "const_buffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* device_context, std::vector<Vertex>& vertices, std::vector<DWORD>& indices);
	Mesh(const Mesh& mesh);
	void Draw();
private:
	VertexBuffer<Vertex> m_vertex_buffer;
	IndexBuffer m_index_buffer;
	ID3D11DeviceContext* m_device_context;
};