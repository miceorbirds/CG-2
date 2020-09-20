#include "katamari_thing.h"
constexpr float RADIUS = 0.5f;

void KatamariThing::CreateUniqueWorld()
{
	SetPosition(getRandomNumber(-500, 500), 0.f, getRandomNumber(-500, 500));
	UpdateMatrix();
	collisionSphere.sphere.Center = this->GetPositionFloat3();
	collisionSphere.sphere.Radius = RADIUS;
	collisionSphere.collision = DISJOINT;
	diff = getRandomNumber(-20, 20);
}

void KatamariThing::CreateKatamari()
{
	this->isKatamari = true;
	collisionSphere.sphere.Center = this->GetPositionFloat3();
	collisionSphere.sphere.Radius = RADIUS;
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

const bool KatamariThing::CheckColision(KatamariThing& gameObject)
{
	auto maxDirOwn = this->GetMaxDirection();
	auto minDirOwn = this->GeMinDirection();
	auto maxDirTo = gameObject.GetMaxDirection();
	auto minDirTo = gameObject.GeMinDirection();
	XMFLOAT3 maxDirOwn_v, minDirOwn_v, maxDirTo_v, minDirTo_v;
	XMStoreFloat3(&maxDirOwn_v, maxDirOwn);
	XMStoreFloat3(&minDirOwn_v, minDirOwn);
	XMStoreFloat3(&maxDirTo_v, maxDirTo);
	XMStoreFloat3(&minDirTo_v, minDirTo);
	if (maxDirTo_v.x >= minDirOwn_v.x
		&& maxDirTo_v.z >= minDirOwn_v.z
		&& minDirTo_v.x <= maxDirOwn_v.x
		&& minDirTo_v.z <= maxDirOwn_v.z)
	{
		return true;
	};
	return false;
}

const bool KatamariThing::CheckFutureColision(KatamariThing& gameObject, XMVECTOR adjustPosition)
{
	auto maxDirOwn = this->GetMaxDirection();
	auto minDirOwn = this->GeMinDirection();
	auto maxDirTo = gameObject.GetMaxDirection() + adjustPosition;
	auto minDirTo = gameObject.GeMinDirection() + adjustPosition;
	XMFLOAT3 maxDirOwn_v, minDirOwn_v, maxDirTo_v, minDirTo_v;
	XMStoreFloat3(&maxDirOwn_v, maxDirOwn);
	XMStoreFloat3(&minDirOwn_v, minDirOwn);
	XMStoreFloat3(&maxDirTo_v, maxDirTo);
	XMStoreFloat3(&minDirTo_v, minDirTo);
	if (maxDirTo_v.x >= minDirOwn_v.x
		&& maxDirTo_v.z >= minDirOwn_v.z
		&& minDirTo_v.x <= maxDirOwn_v.x
		&& minDirTo_v.z <= maxDirOwn_v.z)
	{
		return true;
	};
	return false;
}

const XMVECTOR KatamariThing::GetMaxDirection()
{
	XMVECTOR result = this->m_model.GetMaxDirections() * XMLoadFloat3(&this->m_scale) + XMLoadFloat3(&this->m_pos);
	return result;
}

const XMVECTOR KatamariThing::GeMinDirection()
{
	XMVECTOR result = this->m_model.GetMinDirections() * XMLoadFloat3(&this->m_scale) + XMLoadFloat3(&this->m_pos);
	return result;
}

void KatamariThing::UpdateKatamari()
{
	collisionSphere.sphere.Center = this->GetPositionFloat3();
}