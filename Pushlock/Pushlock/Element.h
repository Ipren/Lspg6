#pragma once

#include "Map.h"

class FireProjectileSpell;
class Player;

class Element {

public:
	Element() : cooldown() { }
	virtual void update(Player *player, Map *map, float dt) {
		for (int i = 0; i < 5; i++)//counting down all cooldowns
		{
			if (cooldown[i] > 0)
				this->cooldown[i] -= dt;
			if (cooldown[i] < 0)
				this->cooldown[i] = 0;
		}
	};
	
	virtual void projectile(Player *player, Map *map) = 0;
	virtual void stomp(Player *player, Map *map) = 0;
	virtual void wall(Player *player, Map *map) = 0;
	virtual void dash(Player *player, Map *map) = 0;

	float cooldown[5];
};

class ArcaneElement : public Element {

public:

	virtual void projectile(Player *player, Map *map) override;
	virtual void stomp(Player *player, Map *map) override;
	virtual void wall(Player *player, Map *map) override;
	virtual void dash(Player *player, Map *map) override;

};

class FireElement : public Element {
public:
	FireElement();

	virtual void projectile(Player *player, Map *map) override;
	virtual void stomp(Player *player, Map *map) override;
	virtual void wall(Player *player, Map *map) override;
	virtual void dash(Player *player, Map *map) override;

	FireProjectileSpell *active_projectile;
};

class WindElement : public Element {

public:

	virtual void projectile(Player *player, Map *map) override;
	virtual void stomp(Player *player, Map *map) override;
	virtual void wall(Player *player, Map *map) override;
	virtual void dash(Player *player, Map *map) override;

};
