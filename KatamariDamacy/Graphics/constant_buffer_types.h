#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX WVP_matrix;
	DirectX::XMMATRIX world_matrix;
};

struct CB_VS_camLight_matrix
{
	DirectX::XMMATRIX camLightViewMatrix;
	DirectX::XMMATRIX camLightProjMatrix;
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
	float padding;
	DirectX::XMFLOAT3 light_direction;
};

struct CB_PS_light_pointbulb
{
	DirectX::XMFLOAT3 cameraPosition;
	DirectX::XMFLOAT3 lightColor;
	DirectX::XMFLOAT3 lightPosition;
	float lightRadius;
};
