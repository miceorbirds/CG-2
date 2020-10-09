#pragma once
#include "../Game/renderable_game_object.h"

class Light : public RenderableGameObject
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	bool Initialize(float screen_near, float screen_depth);

	bool isSun;
	XMFLOAT3 ambient_light_color = XMFLOAT3(0.1f, 0.2f, 0.5f);
	float ambient_light_strength = 0.005f;

	XMFLOAT3 diffuse_light_color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	float diffuse_light_strength = 0.5f;

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();
	XMFLOAT3 GetDirection();
	void SetDirection(float x, float y, float z);
	void UpdateViewMatrix();
	void UpdateViewMatrix(XMFLOAT3 mainObjectPos);

	float specular_strength = 0.5f;

	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;
private:
	XMVECTOR m_direction;

	void GenerateProjectionMatrix(float screen_near, float screen_depth);

	XMMATRIX m_view_matrix;
	XMMATRIX m_projection_matrix;
};