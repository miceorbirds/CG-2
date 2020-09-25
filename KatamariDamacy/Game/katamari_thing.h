#pragma once
#include "renderable_game_object.h"
#include <DirectXCollision.h>

class KatamariThing : public RenderableGameObject
{
public:
	void CreateUniqueWorld();
	void CreateKatamari();
private:
	bool isKatamari = false;
	int getRandomNumber(int min, int max)
	{
		static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
		return static_cast<int>(rand() * fraction * (max - min + 1) + min);
	}
};
