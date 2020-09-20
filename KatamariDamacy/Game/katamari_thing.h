#pragma once
#include "renderable_game_object.h"
#include <DirectXCollision.h>

class KatamariThing : public RenderableGameObject
{
public:
	struct CollisionSphere {
		DirectX::BoundingSphere sphere;
		ContainmentType collision;
	} collisionSphere;
	bool IsGathered = false;
private:
	DirectX::XMVECTOR centerOfCollision;
	int getRandomNumber(int min, int max)
	{
		static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
		return static_cast<int>(rand() * fraction * (max - min + 1) + min);
	}

	float diff = getRandomNumber(-20, 20);
};
