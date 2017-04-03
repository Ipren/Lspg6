#pragma once
#include "Entity.h"
#include "Map.h"
class Spell : public Entity
{
public:
	Spell(XMFLOAT3 position, XMFLOAT2 velocity, float radius, float life);
	virtual ~Spell();

	virtual void update(Map *map, float dt) override {
		position.x += velocity.x * dt;
		position.z += velocity.y * dt;

		//velocity = { 0, 0 };
		life -= dt;
		if (life <= 0.f) {
			dead = true;
		}
	};

	virtual void on_effect(Map *map) {};
private:
	float life;
};

class PushSpell : public Spell
{
public:
	PushSpell(XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~PushSpell();
	
	virtual void on_effect(Map *map) override;
private:
	float explosion_radius;
	float strength;
};
