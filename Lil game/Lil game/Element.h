#pragma once

#include "Map.h"

class Player;

class Element {

public:
	//virtual void update(Player *player, Map *map, float dt) = 0;
	virtual void projectile(Player *player, Map *map) = 0;
	virtual void stomp(Player *player, Map *map) = 0;
	virtual void wall(Player *player, Map *map) = 0;
};

class ArcaneElement : public Element {

public:
	virtual void projectile(Player *player, Map *map) override;
	virtual void stomp(Player *player, Map *map) override;
	virtual void wall(Player *player, Map *map) override;
};