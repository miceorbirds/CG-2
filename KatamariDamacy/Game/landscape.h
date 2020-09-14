#pragma once
#include "game_object.h"
class Landscape
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void Draw(ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void Draw(const XMMATRIX& viewProjectionMatrix);
	void Draw(ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, const XMMATRIX& viewProjectionMatrix);

	void SetTexture(ID3D11ShaderResourceView* texture);
private:
	VertexBuffer<Vertex> m_vertex_buffer;
	IndexBuffer m_index_buffer;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_device_context = nullptr;
	ID3D11ShaderResourceView* m_texture = nullptr;

	XMMATRIX m_world_matrix = XMMatrixIdentity();
};