#include "katamari_thing.h"

constexpr float RADIUS = 0.5f;

void KatamariThing::CreateUniqueWorld()
{
	SetPosition(getRandomNumber(-500, 500), 0.f, getRandomNumber(-500, 500));
	UpdateMatrix();
	float size = 0.1;
	collisionSphere.sphere.Center = this->GetPositionFloat3();
	collisionSphere.sphere.Radius = 10;
	collisionSphere.collision = DISJOINT;
	diff = getRandomNumber(-20, 20);
}

void KatamariThing::CreateKatamari()
{
	this->isKatamari = true;
	float size = 2;
	SetScale(size, size, size);
	collisionSphere.sphere.Center = this->GetPositionFloat3();
	collisionSphere.sphere.Radius = size / 2;
	collisionSphere.collision = DISJOINT;
}

void KatamariThing::Update()
{
	if (collisionSphere.collision == INTERSECTS)
	{
		IsGathered = true;
	}
}

void KatamariThing::DrawAttached(const XMMATRIX& viewProjectionMatrix, XMMATRIX katamari_world)
{
	m_world_matrix = XMMatrixIdentity();
	m_world_matrix = XMMatrixTranslation(2.0f + diff, 2.0f, 2.0f) * XMMatrixScaling(this->m_scale.x, this->m_scale.y, this->m_scale.z);
	m_model.Draw(m_world_matrix * katamari_world, viewProjectionMatrix);
}

void KatamariThing::UpdateKatamari()
{
	collisionSphere.sphere.Center = this->GetPositionFloat3();
}