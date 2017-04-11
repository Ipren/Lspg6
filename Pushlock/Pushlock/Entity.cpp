#include "Entity.h"

Entity::Entity()
{
}

Entity::Entity(EntityType type, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: type(type), position(position), acceleration({ 0.f, 0.f }), velocity(velocity), radius(radius), angle(0.f), dead(false)
{
}

Entity::~Entity()
{
}
