#pragma once

#include "../error_logger.h"
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

class VertexShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements);
	ID3D11VertexShader* GetShader() const;
	ID3D10Blob* GetBuffer() const;
	ID3D11InputLayout* GetInputLayout() const;
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> m_shader_buffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_input_layout;
};

class PixelShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath);
	ID3D11PixelShader* GetShader() const;
	ID3D10Blob* GetBuffer() const;
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> m_shader_buffer;
};
