#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX WVP_matrix;
	DirectX::XMMATRIX world_matrix;
};

struct CB_PS_pixelshader
{
	float alpha = 1.0f;
};

struct CB_PS_light
{
	DirectX::XMFLOAT3 ambient_color;
	float ambient_strength;

	DirectX::XMFLOAT3 diffuse_color;
	float diffuse_strength;

	DirectX::XMFLOAT3 light_position;
	float specular_strength;

	DirectX::XMFLOAT3 camera_position;
};

struct CB_PS_light_pointbulb
{
	DirectX::XMFLOAT3 ambient_light_color;
	float ambient_light_strength;

	DirectX::XMFLOAT3 dynamic_light_color;
	float dynamic_light_strength;
	DirectX::XMFLOAT3 dynamic_light_position;
	float dynamic_light_attenuation_a;
	float dynamic_light_attenuation_b;
	float dynamic_light_attenuation_c;
};