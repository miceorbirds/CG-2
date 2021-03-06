#include "shaders.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->m_shader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreateVertexShader(this->m_shader_buffer->GetBufferPointer(), this->m_shader_buffer->GetBufferSize(), NULL, this->m_shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create vertex shader: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreateInputLayout(layoutDesc, numElements, this->m_shader_buffer->GetBufferPointer(), this->m_shader_buffer->GetBufferSize(), this->m_input_layout.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create input layout.");
		return false;
	}

	return true;
}

ID3D11VertexShader* VertexShader::GetShader() const
{
	return this->m_shader.Get();
}

ID3D10Blob* VertexShader::GetBuffer() const
{
	return this->m_shader_buffer.Get();
}

ID3D11InputLayout* VertexShader::GetInputLayout() const
{

	return this->m_input_layout.Get();
}


bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->m_shader_buffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreatePixelShader(this->m_shader_buffer.Get()->GetBufferPointer(), this->m_shader_buffer.Get()->GetBufferSize(), NULL, this->m_shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create pixel shader: ";
		errorMsg += shaderpath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	return true;
}

ID3D11PixelShader* PixelShader::GetShader() const
{
	return this->m_shader.Get();
}

ID3D10Blob* PixelShader::GetBuffer() const
{
	return this->m_shader_buffer.Get();
}
