#pragma once
#include "Player.h"
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
	pointLight light;
private:
	float explosion_radius;
	float strength;
	
};

class ArcaneWallSpell : public Spell
{
public:
	ArcaneWallSpell(Player *owner, XMFLOAT3 position, float radius);
	virtual ~ArcaneWallSpell();

	virtual void update(Map *map, float dt) override;

	virtual bool on_effect(Map *map) override;// { return false; };
	XMFLOAT2 endPos;
	bool edge;
private:
	/*float explosion_radius;
	float strength;*/
};

class FireProjectileSpell : public Spell
{
public:
	FireProjectileSpell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~FireProjectileSpell();

	virtual void update(Map *map, float dt) override;
	virtual bool on_effect(Map *map) override;
	pointLight light;
private:
	float explosion_radius;
	float strength;

};

class FireWallSpell : public Spell
{
public:
	FireWallSpell(Player *owner, XMFLOAT3 position, float radius);
	virtual ~FireWallSpell();

	virtual void update(Map *map, float dt) override;

	virtual bool on_effect(Map *map) override;// { return false; };
	XMFLOAT2 endPos;
	bool edge;
private:
	/*float explosion_radius;
	float strength;*/
};

class WindProjectileSpell : public Spell
{
public:
	WindProjectileSpell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~WindProjectileSpell();

	virtual void update(Map *map, float dt) override;
	virtual bool on_effect(Map *map) override;
private:
	float strength;
};

class EarthProjectileSpell : public Spell
{
public:
	EarthProjectileSpell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~EarthProjectileSpell();

	virtual void update(Map *map, float dt) override;
	virtual bool on_effect(Map *map) override;
	pointLight light;
private:
	float effect_radius;
	float strength;
	float alive;

};

class EarthWallSpell : public Spell
{
public:
	EarthWallSpell(Player *owner, XMFLOAT3 position, float radius);
	virtual ~EarthWallSpell();

	virtual void update(Map *map, float dt) override;

	virtual bool on_effect(Map *map) override;// { return false; };
	XMFLOAT2 endPos;
	bool edge;
private:
	/*float explosion_radius;
	float strength;*/
};

class WaterProjectileSpell : public Spell
{
public:
	WaterProjectileSpell(Player* owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~WaterProjectileSpell();

	virtual void update(Map *map, float dt) override;
	virtual bool on_effect(Map *map) override;

private:
	float strenght;
};

class WaterIcePatch : public Spell
{
public:
	WaterIcePatch(Player* owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~WaterIcePatch();

	virtual void update(Map *map, float dt) override;
	virtual bool on_effect(Map *map) override;
private:
	float dSpeed = 287.0f;
	float dFriction = -223.0f;
};

class WindFartCloudSpell : public Spell
{
public:
	WindFartCloudSpell(Player* owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~WindFartCloudSpell();

	virtual void update(Map *map, float dt) override;
	virtual bool on_effect(Map *map) override;
};

class WindBeaconSpell : public Spell
{
public:
	WindBeaconSpell(Player* owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~WindBeaconSpell();

	virtual void update(Map *map, float dt) override;
	virtual bool on_effect(Map *map) override;

	bool hasStomped();
	void endStomped();

private:
	float lifetime;
	bool stomped;
};

class FirePathSpell : public Spell
{
public:
	FirePathSpell(Player* owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	~FirePathSpell();

	virtual void update(Map *map, float dt) override;
	virtual bool on_effect(Map *map) override;

private:
	float dot = -0.43f;
};
