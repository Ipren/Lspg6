#pragma once

#include "Entity.h"

class Player : public Entity
{
public:
	Player(unsigned int index);
	virtual ~Player();

	void update(float dt) override;

private:
	unsigned int index;
};
