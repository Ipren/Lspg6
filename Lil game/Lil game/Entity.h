#pragma once
#include <DirectXMath.h>
#include "Definitions.h"
class Entity
{
public:
	Entity();
	virtual ~Entity();

	virtual void update() {}

//private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 velocity;
	float radious;
	float angle;
	EntityType type;
};
