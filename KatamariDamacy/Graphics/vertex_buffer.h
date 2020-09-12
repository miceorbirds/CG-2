#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template<class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	UINT m_stride = sizeof(T);
	UINT m_vertex_count = 0;

public:
	VertexBuffer() {};

	VertexBuffer(const VertexBuffer<T>& rhs)
	{
		this->m_buffer = rhs.m_buffer;
		this->m_vertex_count = rhs.m_vertex_count;
		this->m_stride = rhs.m_stride;
	}

	VertexBuffer<T>& operator=(const VertexBuffer<T>& a)
	{
		this->m_buffer = a.m_buffer;
		this->m_vertex_count = a.m_vertex_count;
		this->m_stride = a.m_stride;
		return *this;
	}

	ID3D11Buffer* Get()const
	{
		return m_buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return m_buffer.GetAddressOf();
	}

	UINT VertexCount() const
	{
		return this->m_vertex_count;
	}

	const UINT Stride() const
	{
		return this->m_stride;
	}

	const UINT* StridePtr() const
	{
		return &this->m_stride;
	}

	HRESULT Initialize(ID3D11Device* device, T* data, UINT vertex_count)
	{
		if (m_buffer.Get() != nullptr)
			m_buffer.Reset();
		this->m_vertex_count = vertex_count;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = m_stride * vertex_count;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->m_buffer.GetAddressOf());
		return hr;
	}
};
