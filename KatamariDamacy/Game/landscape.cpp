#include "landscape.h"
#include "../Inc/WICTextureLoader.h"

bool Landscape::Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context)
{
	this->m_device_context = device_context;
	try
	{
		//Load Texture
		auto hr = CreateWICTextureFromFile(device, L"Data\\Textures\\seamless_grass.jpg", nullptr, &m_texture);
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");
		this->m_device = device;
		this->m_device_context = device_context;
		// Textured Square
		Vertex v[] =
		{
			Vertex(-100,  -0.5f, -100, 10.0f, 10.0f, 0.0f, 1.0f, 0.0f), //FRONT Bottom Left   - [0]
			Vertex(-100,  -0.5f,  100, 0.0f, 10.0f, 0.0f, 1.0f, 0.0f), //FRONT Top Left      - [1]
			Vertex(100,  -0.5f,  100, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f), //FRONT Top Right     - [2]
			Vertex(100,  -0.5f, -100, 10.0f, 0.0f, 0.0f, 1.0f, 0.0f), //FRONT Bottom Right   - [3]
		};
		// Load vertex data
		hr = this->m_vertex_buffer.Initialize(device, v, ARRAYSIZE(v));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer.");
		DWORD indices[] =
		{
			0, 1, 2, //FRONT
			0, 2, 3, //FRONT
		};
		// Load index data
		hr = this->m_index_buffer.Initialize(device, indices, ARRAYSIZE(indices));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Landscape::Draw(const XMMATRIX& viewProjectionMatrix)
{
	UINT offset = 0;
	this->m_device_context->IASetVertexBuffers(0, 1, this->m_vertex_buffer.GetAddressOf(), this->m_vertex_buffer.StridePtr(), &offset);
	this->m_device_context->IASetIndexBuffer(this->m_index_buffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->m_device_context->DrawIndexed(this->m_index_buffer.IndexCount(), 0, 0);
}

void Landscape::Draw(ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, const XMMATRIX& viewProjectionMatrix)
{
	this->m_device_context->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf());
	//Update Constant buffer with WVP Matrix
	cb_vs_vertexshader.data.world_matrix = this->m_world_matrix;
	cb_vs_vertexshader.data.WVP_matrix = viewProjectionMatrix;
	cb_vs_vertexshader.ApplyChanges();

	this->m_device_context->PSSetShaderResources(0, 1, &this->m_texture); //Set Texture

	UINT offset = 0;
	this->m_device_context->IASetVertexBuffers(0, 1, this->m_vertex_buffer.GetAddressOf(), this->m_vertex_buffer.StridePtr(), &offset);
	this->m_device_context->IASetIndexBuffer(this->m_index_buffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->m_device_context->DrawIndexed(this->m_index_buffer.IndexCount(), 0, 0);
}

void Landscape::SetTexture(ID3D11ShaderResourceView* texture)
{
	this->m_texture = texture;
}