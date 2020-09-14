#include "directional_light.h"

bool DirectionalLight::Initialize(float screenNear, float screenDepth)
{
	this->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	this->SetDirection(1, -1, 0);
	//this->GenerateProjectionMatrix(screenNear, screenDepth);
	this->specular_power = 25.0f;
	return true;
}

bool DirectionalLight::Initialize()
{
	this->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	this->SetDirection(1, -1, 0);
	return true;
}

void DirectionalLight::SetColor(float red, float green, float blue, float strength)
{
	this->light_color = DirectX::XMFLOAT3(red, green, blue);
	this->light_strength = strength;
}

void DirectionalLight::SetDirection(float x, float y, float z)
{
	this->dir = XMFLOAT3(x, y, z);
}

XMFLOAT3 DirectionalLight::GetDirectionalColor()
{
	return this->light_color;
}

float DirectionalLight::GetStrength()
{
	return this->light_strength;
}

XMFLOAT3 DirectionalLight::GetDirection()
{
	return this->dir;
}

XMMATRIX DirectionalLight::GetViewMatrix()
{
	return this->viewMatrix;
}

XMMATRIX DirectionalLight::GetProjectionMatrix()
{
	return this->projMatrix;
}

//void DirectionalLight::UpdateViewMatrix(Vector3 mainObjectPos)
//{
//	Vector3 shadowCamPos = mainObjectPos - this->dir * 10;
//
//	//Vector3 shadowCamPos = mainObjectPos + Vector3::Up * 10;
//
//	this->viewMatrix = Matrix::CreateLookAt(
//		shadowCamPos,
//		mainObjectPos,
//		Vector3::Right
//	);
//}

//void DirectionalLight::AdjustTime(float dt)
//{
//	if (this->dir.x + dt > 1)
//	{
//		this->dir.x = -1;
//
//		this->light_strength = this->isNight ? 1.0f : 0.3f;
//		this->isNight = !this->isNight;
//	}
//}

//void DirectionalLight::GenerateProjectionMatrix(float screenNear, float screenDepth)
//{
//	float fieldOfView = (float)XM_PI / 2.0f;
//	float screenAspect = 1.0f;
//	//this->projMatrix = Matrix::CreatePerspectiveFieldOfView(fieldOfView, screenAspect, screenNear, screenDepth);
//	this->projMatrix = Matrix::CreateOrthographic(32, 32, screenNear, screenDepth);
//}