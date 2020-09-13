#pragma once

#include <DirectXMath.h>
#include "../Game/game_object.h"
using namespace DirectX;

class Camera : public GameObject
{
public:
	Camera();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;
private:
	void UpdateMatrix();

	XMMATRIX m_view_matrix;
	XMMATRIX m_projection_matrix;
};