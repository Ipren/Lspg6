#pragma once

#include "Entity.h"

class Player : public Entity
{
public:
	Player(unsigned int index);
	virtual ~Player();

	void update() override;

private:
	unsigned int index;
};
