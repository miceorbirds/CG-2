#pragma once
#include "../Game/renderable_game_object.h"

class Light : public RenderableGameObject
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	bool Initialize(ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);

	bool isSun;
	DirectX::XMFLOAT3 ambient_light_color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float ambient_light_strength = 1.0f;

	DirectX::XMFLOAT3 diffuse_light_color = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	float diffuse_light_strength = 1.0f;

	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;
};