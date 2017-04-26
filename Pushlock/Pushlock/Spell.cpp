#include "Spell.h"

#include "Constants.h"
#include "Player.h"
#include "Upgrades.h"

Spell::Spell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius, float life)
	: Entity(EntityType::Spell, position, velocity, radius), owner(owner), life(life)
{
}

Spell::~Spell()
{
}

ArcaneProjectileSpell::ArcaneProjectileSpell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(owner, position, velocity, radius, 4.5f + gPlayerSpellConstants[owner->index].kArcaneProjectileLifeTime), explosion_radius(1.5f), strength(1.f)
{
	this->pEmitter.randomVector = DirectX::XMFLOAT4(velocity.x, position.x, velocity.y, 1.0f);
	this->pEmitter.position = position;
	this->pEmitter.particleType = 0;
	this->light.lightColor = XMFLOAT4(0.28f, 0.1f, 0.56f, 1.0f);
	this->light.lightPos = position;
	this->light.range = 0.9f;
}

ArcaneProjectileSpell::~ArcaneProjectileSpell()
{
	
}

void ArcaneProjectileSpell::update(Map *map, float dt)
{
	EntityQueryResult result;
	if (map->get_nearest_entity(this, gSpellConstants.kArcaneProjectileSeekRadius + gPlayerSpellConstants[owner->index].kArcaneProjectileSeekRadius, &result, [this](Entity *e) {
		return e->type == EntityType::Player && e != (Entity*)this->owner;
	})) {
		float magnitude = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
		
		float ang = atan2f(velocity.y, velocity.x);
		float an = atan2(sin(result.angle - ang), cos(result.angle - ang));

		float new_angle = fmod((ang + an *
			(gSpellConstants.kArcaneProjectileSeekStrength + gPlayerSpellConstants[owner->index].kArcaneProjectileSeekStrength 
				+ abs(gSpellConstants.kArcaneProjectileSeekRadius + gPlayerSpellConstants[owner->index].kArcaneProjectileSeekRadius - result.distance) 
				* (gSpellConstants.kArcaneProjectileSeekFalloff + gPlayerSpellConstants[owner->index].kArcaneProjectileSeekFalloff)) * dt), (XM_PI * 2));
		angle = new_angle;
		
		XMFLOAT2 new_vel = {
			cos(new_angle) * magnitude,
			sin(new_angle) * magnitude
		};

		velocity = new_vel;
	}
	
	Spell::update(map, dt);
	this->light.lightPos = this->position;
}

bool ArcaneProjectileSpell::on_effect(Map *map)
{
	auto nearby = map->get_entities_in_radius(this, explosion_radius, [](Entity *e) {
		return e->type == EntityType::Player;
	});

	for (auto result : nearby) {
		result.entity->velocity.x += cos(result.angle) * (gSpellConstants.kArcaneProjectileStrength + gPlayerSpellConstants[owner->index].kArcaneProjectileStrength) * abs(explosion_radius - result.distance);
		result.entity->velocity.y += sin(result.angle) * (gSpellConstants.kArcaneProjectileStrength + gPlayerSpellConstants[owner->index].kArcaneProjectileStrength) * abs(explosion_radius - result.distance);
	}
	
	return true;
}

bool ArcaneWallSpell::on_effect(Map *map) {
	auto nearby = map->get_entities_in_radius(this, radius, [](Entity *e) {
		return true;
	});

	for (auto result : nearby) {
		result.entity->acceleration.x = -(position.x - result.entity->position.x) * 150;
		result.entity->acceleration.y = -(position.z - result.entity->position.z) * 150;

		//result.entity->velocity.x += cos(result.angle) * 25 * abs(explosion_radius - result.distance);
		//result.entity->velocity.y += sin(result.angle) * 25 * abs(explosion_radius - result.distance);
	}
	
	return false;
};

FireProjectileSpell::FireProjectileSpell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(owner, position, velocity, radius, 4.5f), explosion_radius(1.5f), strength(1.f)
{
	this->light.lightColor = XMFLOAT4(0.6f, 0.1f, 0.1f, 1.0f);
	this->light.lightPos = position;
	this->light.range = 0.9f;
}

FireProjectileSpell::~FireProjectileSpell()
{
}

void FireProjectileSpell::update(Map *map, float dt)
{
	Spell::update(map, dt);
	this->light.lightPos = this->position;
}

bool FireProjectileSpell::on_effect(Map *map)
{
	//saves nearby players in a vector
	auto nearby = map->get_entities_in_radius(this, gSpellConstants.kFireProjectileExplosionRadius + gPlayerSpellConstants[owner->index].kFireProjectileExplosionRadius, [](Entity *e) {
		return e->type == EntityType::Player;
	});

	for (auto result : nearby) { //moves all nearby players
		float falloff = (abs(gSpellConstants.kFireProjectileExplosionRadius + gPlayerSpellConstants[owner->index].kFireProjectileExplosionRadius - result.distance)
			/ (gSpellConstants.kFireProjectileExplosionRadius + gPlayerSpellConstants[owner->index].kFireProjectileExplosionRadius)) 
			* (gSpellConstants.kFireProjectileExplosionFalloff + gPlayerSpellConstants[owner->index].kFireProjectileExplosionFalloff);

		result.entity->velocity.x += cos(result.angle) * ((gSpellConstants.kFireProjectileStrength + gPlayerSpellConstants[owner->index].kFireProjectileStrength) * falloff);
		result.entity->velocity.y += sin(result.angle) * ((gSpellConstants.kFireProjectileStrength + gPlayerSpellConstants[owner->index].kFireProjectileStrength) * falloff);

		if (pUpgrades[this->owner->index].choice[0] == 1 && dynamic_cast<Player *>(result.entity) != this->owner)
		{
			dynamic_cast<Player *>(result.entity)->debuffs.dot = -0.5f;
			dynamic_cast<Player *>(result.entity)->debuffs.duration = 2;
		}
	}
	map->sounds.play(spellSounds::fireExplotion, 0.0f, 50.0f);
	if (this->owner->blowUp == false)
	{
		dynamic_cast<FireElement*>(this->owner->element)->active_projectile->dead = true;
		dynamic_cast<FireElement*>(this->owner->element)->active_projectile = nullptr;
	}
	return true;
}

WindProjectileSpell::WindProjectileSpell(Player * owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(owner, position, velocity, radius, 4.5f), strength(1.f)
{
}

WindProjectileSpell::~WindProjectileSpell()
{
}

void WindProjectileSpell::update(Map * map, float dt)
{
	Spell::update(map, dt);
}

bool WindProjectileSpell::on_effect(Map * map)
{
	auto nearby = map->get_entities_in_radius(this, radius, [](Entity *e) {
		return e->type == EntityType::Player;
	});

	for (auto result : nearby) {
		result.entity->velocity.x += cos(result.angle) * (gSpellConstants.kWindProjectileStrength + gPlayerSpellConstants[owner->index].kWindProjectileStrength);
		result.entity->velocity.y += sin(result.angle) * (gSpellConstants.kWindProjectileStrength + gPlayerSpellConstants[owner->index].kWindProjectileStrength);
	}

	return true;
}

EarthProjectileSpell::EarthProjectileSpell(Player * owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(owner, position, velocity, radius, 4.5f), effect_radius(1.5f), strength(1.f), alive(0.f)
{
	this->light.lightPos = position;
	this->light.lightColor = XMFLOAT4(0.8, 0.8, 0.8, -1.0f);
	this->light.range = 0.9f;
}

EarthProjectileSpell::~EarthProjectileSpell()
{
}

void EarthProjectileSpell::update(Map * map, float dt)
{
	//saves nearby players in a vector
	if (alive > gSpellConstants.kEarthProjectileEffectArmingTime + gPlayerSpellConstants[owner->index].kEarthProjectileEffectArmingTime)
	{
		auto nearby = map->get_entities_in_radius(this, gSpellConstants.kEarthProjectileEffectRadius + gPlayerSpellConstants[owner->index].kEarthProjectileEffectRadius, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby) { //moves all nearby players
			float falloff = (abs(gSpellConstants.kEarthProjectileEffectRadius + gPlayerSpellConstants[owner->index].kEarthProjectileEffectRadius - result.distance)
				/ (gSpellConstants.kEarthProjectileEffectRadius + gPlayerSpellConstants[owner->index].kEarthProjectileEffectRadius))
				* (gSpellConstants.kEarthProjectileEffectFalloff + gPlayerSpellConstants[owner->index].kEarthProjectileEffectFalloff);

			result.entity->velocity.x -= cos(result.angle) * ((gSpellConstants.kEarthProjectileStrength + gPlayerSpellConstants[owner->index].kEarthProjectileStrength) * falloff);
			result.entity->velocity.y -= sin(result.angle) * ((gSpellConstants.kEarthProjectileStrength + gPlayerSpellConstants[owner->index].kEarthProjectileStrength) * falloff);
		}
	}

	alive += dt;
	Spell::update(map, dt);
	this->light.lightPos = this->position;
}

bool EarthProjectileSpell::on_effect(Map * map)
{
	return false;
}

WaterProjectileSpell::WaterProjectileSpell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(owner, position, velocity, radius, 4.5f), strenght(1.0f)
{
	this->pEmitter.position = position;
	this->pEmitter.particleType = 2;
	this->pEmitter.randomVector = XMFLOAT4(velocity.x, 0.0f, velocity.y, 1.0f);
}

WaterProjectileSpell::~WaterProjectileSpell()
{
}

void WaterProjectileSpell::update(Map * map, float dt)
{
	Spell::update(map, dt);
}

bool WaterProjectileSpell::on_effect(Map * map)
{

	auto nearby = map->get_entities_in_radius(this, radius, [](Entity *e) {
		return e->type == EntityType::Player;
	});

	for (auto result : nearby) {
		result.entity->velocity.x += cos(result.angle) * (gSpellConstants.kWaterProjectileStrenght + gPlayerSpellConstants[owner->index].kWaterProjectileStrenght);
		result.entity->velocity.y += sin(result.angle) * (gSpellConstants.kWaterProjectileStrenght + gPlayerSpellConstants[owner->index].kWaterProjectileStrenght);
		if (pUpgrades[this->owner->index].choice[0] == 1)
		{
			dynamic_cast<Player *>(result.entity)->debuffs.speed = -1.0f;
			dynamic_cast<Player *>(result.entity)->debuffs.duration = 5;
		}
		

	}
	dynamic_cast<WaterElement*>(this->owner->element)->active_projectile = nullptr;
	return true;
}
