#pragma once
#include "../Game/renderable_game_object.h"

class PointLightMesh : public RenderableGameObject
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* device_context, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, int pointlightnumber);
	XMFLOAT3 GetLightColor();
	float GetLightRadius();
	void UpdateMatrix();
private:
	XMFLOAT3 _lightColor;
	float m_light_radius;

	float getRandomNumber(float min, float max)
	{
		static float r3 = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
		return r3;
	}
	int getRandomNumber(int min, int max)
	{
		static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
		return static_cast<int>(rand() * fraction * (max - min + 1) + min);
	}
};