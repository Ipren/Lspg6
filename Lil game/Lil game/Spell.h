#pragma once
#include "Entity.h"
#include "Map.h"
class Spell : public Entity
{
public:
	Spell(XMFLOAT3 position, XMFLOAT2 velocity, float radius, float life);
	virtual ~Spell();

	virtual void update(Map *map, float dt) override {
		velocity.x += acceleration.x;
		velocity.y += acceleration.y;

		position.x += velocity.x * dt;
		position.z += velocity.y * dt;

		//velocity.x -= velocity.x * 0.2;
		//velocity.y -= velocity.y * 0.2;

		acceleration.x = 0;
		acceleration.y = 0;

		life -= dt;
		if (life <= 0.f) {
			dead = true;
		}
	};

	virtual void on_effect(Map *map) {};
protected:
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

class WallSpell : public Spell
{
public:
	WallSpell(XMFLOAT3 position, float radius)
		:Spell(position, {0,0}, radius, 10.f){}
	virtual ~WallSpell() {}

	virtual void update(Map *map, float dt) override
	{
		

		life -= dt;
		if (life <= 0.f) {
			dead = true;
		}
	}
private:
	float explosion_radius;
	float strength;
};