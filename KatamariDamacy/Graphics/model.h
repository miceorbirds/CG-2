#pragma once

#include "vertex.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "const_buffer.h"

using namespace DirectX;

class Model
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void SetTexture(ID3D11ShaderResourceView* texture);
	void Draw(const XMMATRIX& view_projection_matrix) const;
private:
	void UpdateWorldMatrix();

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_device_context = nullptr;
	ConstantBuffer<CB_VS_vertexshader>* m_cb_vs_vertexshader = nullptr;
	ID3D11ShaderResourceView* m_texture = nullptr;

	VertexBuffer<Vertex> m_vertex_buffer;
	IndexBuffer m_index_buffer;

	XMMATRIX m_world_matrix = XMMatrixIdentity();
};