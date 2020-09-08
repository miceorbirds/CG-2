#include "model.h"

bool Model::Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->m_device = device;
	this->m_device_context = device_context;
	this->m_texture = texture;
	this->m_cb_vs_vertexshader = &cb_vs_vertexshader;

	try
	{
		//Textured Square
		Vertex v[] =
		{
			Vertex(-0.5f,  -0.5f, -0.5f, 0.0f, 1.0f), //FRONT Bottom Left   - [0]
			Vertex(-0.5f,   0.5f, -0.5f, 0.0f, 0.0f), //FRONT Top Left      - [1]
			Vertex(0.5f,   0.5f, -0.5f, 1.0f, 0.0f), //FRONT Top Right     - [2]
			Vertex(0.5f,  -0.5f, -0.5f, 1.0f, 1.0f), //FRONT Bottom Right   - [3]
			Vertex(-0.5f,  -0.5f, 0.5f, 0.0f, 1.0f), //BACK Bottom Left   - [4]
			Vertex(-0.5f,   0.5f, 0.5f, 0.0f, 0.0f), //BACK Top Left      - [5]
			Vertex(0.5f,   0.5f, 0.5f, 1.0f, 0.0f), //BACK Top Right     - [6]
			Vertex(0.5f,  -0.5f, 0.5f, 1.0f, 1.0f), //BACK Bottom Right   - [7]
		};

		//Load Vertex Data
		HRESULT hr = this->m_vertex_buffer.Initialize(this->m_device, v, ARRAYSIZE(v));
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

		//Load Index Data
		hr = this->m_index_buffer.Initialize(this->m_device, indices, ARRAYSIZE(indices));
		COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer.");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	this->UpdateWorldMatrix();
	return true;
}

void Model::SetTexture(ID3D11ShaderResourceView* texture)
{
	this->m_texture = texture;
}

void Model::Draw(const XMMATRIX& view_projection_matrix) const
{
	//Update Constant buffer with WVP Matrix
	this->m_cb_vs_vertexshader->data.mat = this->m_world_matrix * view_projection_matrix; //Calculate World-View-Projection Matrix
	this->m_cb_vs_vertexshader->data.mat = XMMatrixTranspose(this->m_cb_vs_vertexshader->data.mat);
	this->m_cb_vs_vertexshader->ApplyChanges();
	this->m_device_context->VSSetConstantBuffers(0, 1, this->m_cb_vs_vertexshader->GetAddressOf());

	this->m_device_context->PSSetShaderResources(0, 1, &this->m_texture); //Set Texture
	this->m_device_context->IASetIndexBuffer(this->m_index_buffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	UINT offset = 0;
	this->m_device_context->IASetVertexBuffers(0, 1, this->m_vertex_buffer.GetAddressOf(), this->m_vertex_buffer.StridePtr(), &offset);
	this->m_device_context->DrawIndexed(this->m_index_buffer.BufferSize(), 0, 0); //Draw
}

void Model::UpdateWorldMatrix()
{
	this->m_world_matrix = XMMatrixIdentity();
}