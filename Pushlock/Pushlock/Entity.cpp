#include "Entity.h"

Entity::Entity()
{
}

Entity::Entity(EntityType type, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: type(type), position(position), acceleration({ 0.f, 0.f }), velocity(velocity), radius(radius), angle(0.f), dead(false)
{
}

void Entity::AssignStaticMesh(string meshName)
{
	this->pMesh = MeshContainer::staticMeshes[meshName];
}

void Entity::update(Map * map, float dt)
{
	if (this->pAnimator != nullptr)
		pAnimator->Update(dt);
}

Entity::~Entity()
{
	if (this->pMesh != nullptr)
		delete pMesh;
}
