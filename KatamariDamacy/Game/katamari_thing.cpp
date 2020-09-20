#include "katamari_thing.h"

void KatamariThing::CreateUniqueWorld()
{
	SetPosition(getRandomNumber(-500, 500), 0.f, getRandomNumber(-500, 500));
	UpdateMatrix();
}