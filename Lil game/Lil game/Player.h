#pragma once

#include "Entity.h"
#include "Map.h"
#include "Element.h"

class Player : public Entity
{
public:
	Player(unsigned int index, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~Player();

	void update(Map *map, float dt) override;

private:
	Element *element;
	unsigned int index;
	float cooldown[5];
};
