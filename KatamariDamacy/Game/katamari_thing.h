#pragma once
#include "renderable_game_object.h"
#include <DirectXCollision.h>
#include "../Graphics/debug_draw.h"

class KatamariThing : public RenderableGameObject
{
public:
	struct CollisionSphere {
		BoundingSphere sphere;
		ContainmentType collision;
	} collisionSphere;
	bool IsGathered = false;

	void CreateUniqueWorld();
	void CreateKatamari();
	void Update();
	void UpdateKatamari();
	void DrawAttached(const XMMATRIX& viewProjectionMatrix, XMMATRIX katamari_world);

	const bool CheckColision(KatamariThing& gameObject);
	const bool CheckFutureColision(KatamariThing& gameObject, XMVECTOR adjustPosition);
	const XMVECTOR GetMaxDirection();
	const XMVECTOR GeMinDirection();

private:

	bool isKatamari = false;

	//DirectX::XMVECTOR centerOfCollision;
	int getRandomNumber(int min, int max)
	{
		static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
		return static_cast<int>(rand() * fraction * (max - min + 1) + min);
	}

	float diff{ 1 };
};
