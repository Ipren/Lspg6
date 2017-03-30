#pragma once
#include <DirectXMath.h>
#include "Definitions.h"
class Entity
{
public:
	Entity();
	virtual ~Entity();

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 velocity;
	EntityType type;
};
