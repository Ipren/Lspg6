#pragma once
#include <DirectXMath.h>
#include "Definitions.h"
class Entity
{
public:
	Entity();
	virtual ~Entity();
	void reset_velocity() { this->velocity.x = 0; this->velocity.y = 0; }
	void add_velocity(float x, float y) { this->velocity.x += x; this->velocity.y += y; }
	DirectX::XMFLOAT3 position;
	float radius;

	virtual void update(float dt) {}

	//private:
	DirectX::XMFLOAT2 velocity;
	float angle;
	EntityType type;
private:
};
