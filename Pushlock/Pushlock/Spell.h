#pragma once
#include "Entity.h"
#include "Map.h"

class Player;

class Spell : public Entity
{
public:
	Spell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius, float life);
	virtual ~Spell();

	virtual void update(Map *map, float dt) override {
		velocity.x += acceleration.x;
		velocity.y += acceleration.y;

		position.x += velocity.x * dt;
		position.z += velocity.y * dt;

		pEmitter.position.x = position.x;
		pEmitter.position.z = position.z;
		pEmitter.particleType = 0;
		this->pEmitter.randomVector = DirectX::XMFLOAT4(velocity.x*0.12, 0, velocity.y*0.12, 1.0f);

		velocity.x -= velocity.x * 0.3f * dt;
		velocity.y -= velocity.y * 0.3f * dt;

		acceleration.x = 0;
		acceleration.y = 0;

		life -= dt;
		if (life <= 0.f) {
			dead = true;
		}
	};

	virtual bool on_effect(Map *map) { return true; };
protected:
	Player *owner;
	float life;
public:
	Emitterlocation pEmitter;
};

class ArcaneProjectileSpell : public Spell
{
public:
	ArcaneProjectileSpell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~ArcaneProjectileSpell();

	virtual void update(Map *map, float dt) override;
	virtual bool on_effect(Map *map) override;
private:
	float explosion_radius;
	float strength;
	
};

class ArcaneWallSpell : public Spell
{
public:
	ArcaneWallSpell(Player *owner, XMFLOAT3 position, float radius)
		:Spell(owner, position, { 0,0 }, radius, 10.f) {
		this->type = EntityType::Wall;}
	virtual ~ArcaneWallSpell() {}

	virtual void update(Map *map, float dt) override
	{
		life -= dt;
		if (life <= 0.f) {
			dead = true;
		}
	}

	virtual bool on_effect(Map *map) override;// { return false; };
private:
	float explosion_radius;
	float strength;
};

class FireProjectileSpell : public Spell
{
public:
	FireProjectileSpell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~FireProjectileSpell();

	virtual void update(Map *map, float dt) override;
	virtual bool on_effect(Map *map) override;
private:
	float explosion_radius;
	float strength;

};