#include "mesh.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* device_context, std::vector<Vertex>& vertices, std::vector<DWORD>& indices)
{
	this->m_device_context = device_context;

	HRESULT hr = this->m_vertex_buffer.Initialize(device, vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->m_index_buffer.Initialize(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
}

Mesh::Mesh(const Mesh& mesh)
{
	this->m_device_context = mesh.m_device_context;
	this->m_index_buffer = mesh.m_index_buffer;
	this->m_vertex_buffer = mesh.m_vertex_buffer;
}

void Mesh::Draw()
{
	UINT offset = 0;
	this->m_device_context->IASetVertexBuffers(0, 1, this->m_vertex_buffer.GetAddressOf(), this->m_vertex_buffer.StridePtr(), &offset);
	this->m_device_context->IASetIndexBuffer(this->m_index_buffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->m_device_context->DrawIndexed(this->m_index_buffer.BufferSize(), 0, 0);
}