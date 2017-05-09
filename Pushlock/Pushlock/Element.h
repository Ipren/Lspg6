#pragma once

#include "Map.h"
#include "ParticleSystem.h"

class FireProjectileSpell;
class WaterProjectileSpell;
class WindBeaconSpell;
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
	float startHealth;
};

class ArcaneElement : public Element {

public:
	ArcaneElement() {
		static ParticleEffect trail = FXSystem->GetFX("arcane-dash-trail");
		dash_trail = trail;

		this->startHealth = 10.f;
	}
	virtual void projectile(Player *player, Map *map) override;
	virtual void stomp(Player *player, Map *map) override;
	virtual void wall(Player *player, Map *map) override;
	virtual void dash(Player *player, Map *map) override;
	virtual void update(Player *player, Map *map, float dt) override;

	XMFLOAT3 returnPos;
	ParticleEffect dash_trail;
	bool teleported = false;
};

class FireElement : public Element {
public:
	FireElement();

	virtual void projectile(Player *player, Map *map) override;
	virtual void stomp(Player *player, Map *map) override;
	virtual void wall(Player *player, Map *map) override;
	virtual void dash(Player *player, Map *map) override;

	FireProjectileSpell *active_projectile;
	int firePatchCount;
	float time = 0.0f;
};

class WindElement : public Element {

public:
	WindElement() {
		this->startHealth = 10.f;}
	virtual void projectile(Player *player, Map *map) override;
	virtual void stomp(Player *player, Map *map) override;
	virtual void wall(Player *player, Map *map) override;
	virtual void dash(Player *player, Map *map) override;

private:
	bool beaconOut = false;
	WindBeaconSpell *spell = nullptr;

};

class EarthElement : public Element {

public:
	EarthElement() {
		this->startHealth = 10.f;}
	virtual void projectile(Player *player, Map *map) override;
	virtual void stomp(Player *player, Map *map) override;
	virtual void wall(Player *player, Map *map) override;
	virtual void dash(Player *player, Map *map) override;
};

class WaterElement : public Element {

public:
	WaterElement() {
		this->startHealth = 10.f;}
	virtual void projectile(Player *player, Map *map) override;
	virtual void stomp(Player *player, Map *map) override;
	virtual void wall(Player *player, Map *map) override;
	virtual void dash(Player *player, Map *map) override;

	WaterProjectileSpell *active_projectile;
	int icePatchCount;
	float time = 0.0f;
};
