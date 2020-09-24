#include "katamari_thing.h"

constexpr float RADIUS = 0.5f;

void KatamariThing::CreateUniqueWorld()
{
	SetPosition(getRandomNumber(-50, 50), 0.f, getRandomNumber(-50, 50));
	this->SetSize(0.1);
	this->SetRotation(0, 0, 0);
	this->radius_bound = 1 * this->m_size;
	UpdateMatrix();
}

void KatamariThing::CreateKatamari()
{
	this->isKatamari = true;
	this->SetSize(2);
	UpdateMatrix();
}