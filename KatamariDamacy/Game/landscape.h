#pragma once
#include "renderable_game_object.h"
class Landcape : public RenderableGameObject
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void Draw(const XMMATRIX& viewProjectionMatrix);
private:
	VertexBuffer<Vertex> m_vertex_buffer;
	IndexBuffer m_index_buffer;
	ID3D11DeviceContext* m_device_context;
};