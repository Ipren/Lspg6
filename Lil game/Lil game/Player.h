#pragma once

#include "Entity.h"
#include "Map.h"

class Player : public Entity
{
public:
	Player(unsigned int index, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~Player();

	void update(Map *map, float dt) override;

private:
	unsigned int index;
};
