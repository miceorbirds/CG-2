#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class IndexBuffer
{
private:
	IndexBuffer(const IndexBuffer& rhs);

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	UINT m_buffer_size = 0;
public:
	IndexBuffer() {}

	ID3D11Buffer* Get()const
	{
		return m_buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return m_buffer.GetAddressOf();
	}

	UINT BufferSize() const
	{
		return this->m_buffer_size;
	}

	HRESULT Initialize(ID3D11Device* device, DWORD* data, UINT numIndices)
	{
		this->m_buffer_size = numIndices;
		//Load Index Data
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndices;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = data;
		HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, m_buffer.GetAddressOf());
		return hr;
	}
};