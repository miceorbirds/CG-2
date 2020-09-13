#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX mat;
};

struct CB_PS_pixelshader
{
	float alpha = 1.0f;
};
struct CB_PS_light
{
	DirectX::XMFLOAT3 ambient_light_color;
	float ambient_light_strength;
};