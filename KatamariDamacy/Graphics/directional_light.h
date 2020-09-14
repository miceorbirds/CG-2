#pragma once
#include "../Game/renderable_game_object.h"
#include "color.h"

class DirectionalLight
{
public:
	bool Initialize(float, float);
	bool Initialize();
	void SetColor(float, float, float, float);
	void SetDirection(float, float, float);

	XMFLOAT3 GetDirectionalColor();
	float GetStrength();
	XMFLOAT3 GetDirection();

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

	//void UpdateViewMatrix(XMMATRIX);
	//void AdjustTime(float);
	float specular_power;

private:
	//void GenerateProjectionMatrix(float, float);
	//bool isNight;
	DirectX::XMFLOAT3 light_color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float light_strength = 1.f;
	XMFLOAT3 dir;

	XMMATRIX viewMatrix;
	XMMATRIX projMatrix;
};