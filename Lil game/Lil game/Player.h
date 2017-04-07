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
	/*0:projectile
	1:dash
	2:stomp
	3:wall
	4:defence*/
	float cooldown[5];
};
