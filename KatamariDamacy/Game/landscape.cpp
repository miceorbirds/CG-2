#include "landscape.h"

bool Landcape::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->m_device_context = deviceContext;
	try
	{
		// Textured Square
		Vertex v[] =
		{
			Vertex(-1.0f, -1.0f, -1.0f, 100.0f, 100.0f, 0.0f, 1.0f, 0.0f),
			Vertex(1.0f, -1.0f, -1.0f,   0.0f, 100.0f, 0.0f, 1.0f, 0.0f),
			Vertex(1.0f, -1.0f,  1.0f,   0.0f,   0.0f, 0.0f, 1.0f, 0.0f),
			Vertex(-1.0f, -1.0f,  1.0f, 100.0f,   0.0f, 0.0f, 1.0f, 0.0f),
		};
		// Load vertex data
		auto hr = this->m_vertex_buffer.Initialize(device, v, ARRAYSIZE(v));
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
		// Load index data
		hr = this->m_index_buffer.Initialize(device, indices, ARRAYSIZE(indices));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");
		this->SetPosition(0.0f, 0.0f, 0.0f);
		this->SetRotation(0.0f, 0.0f, 0.0f);
		this->UpdateMatrix();
		
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Landcape::Draw(const XMMATRIX& viewProjectionMatrix)
{
	UINT offset = 0;
	this->m_device_context->IASetVertexBuffers(0, 1, this->m_vertex_buffer.GetAddressOf(), this->m_vertex_buffer.StridePtr(), &offset);
	this->m_device_context->IASetIndexBuffer(this->m_index_buffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->m_device_context->DrawIndexed(this->m_index_buffer.IndexCount(), 0, 0);
}
