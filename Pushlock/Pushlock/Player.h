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
	bool stomped;
	unsigned int index;
	bool blowUp;
	bool ready;

//private:
	float health;
	float maxHealth;
	Element *element;
	debuf debuffs;
	/*0:projectile
	1:dash
	2:stomp
	3:wall
	4:defence*/
	//float cooldown[5];
};
