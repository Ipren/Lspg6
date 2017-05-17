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
	static ParticleEffect ArcaneTrail = FXSystem->GetFX("arcane-proj-trail");

	this->pEmitter.randomVector = DirectX::XMFLOAT4(velocity.x, position.x, velocity.y, 1.0f);
	this->pEmitter.position = position;
	this->pEmitter.particleType = 0;
	this->light.lightColor = XMFLOAT4(0.8f, 0.78f, 1.0f, 1.0f);
	this->light.lightPos = position;
	this->light.range = 1.4f;
	this->trail = ArcaneTrail;
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
	FXSystem->ProcessFX(this->trail, XMMatrixTranslation(position.x, position.y, position.z), dt);
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
	
	FXSystem->AddFX("splosion", XMMatrixTranslation(position.x, position.y, position.z));
	map->sounds.play(spellSounds::arcaneHit, 0.0f, 14.0f);
	return true;
}

ArcaneWallSpell::ArcaneWallSpell(Player *owner, XMFLOAT3 position, float radius)
	:Spell(owner, position, { 0,0 }, radius, 10.f) 
{
	this->endPos = endPos;
	this->type = EntityType::Wall;
	this->angle = owner->angle;
	this->edge = false;
}

ArcaneWallSpell::~ArcaneWallSpell()
{
}

void ArcaneWallSpell::update(Map * map, float dt)
{
	life -= dt;
	if (life <= 0.f) {
		dead = true;
	}
}

bool ArcaneWallSpell::on_effect(Map *map) {
	auto nearby = map->get_entities_in_radius(this, radius, [](Entity *e) {
		return true;
	});

	for (auto result : nearby)
	{
		if (result.entity->type != this->type)
		{
			float dx = abs(this->position.x - result.entity->position.x);
			float dz = abs(this->position.z - result.entity->position.z);

			float distance = sqrt(dx * dx + dz * dz);

			WindFartCloudSpell* f = dynamic_cast<WindFartCloudSpell*>(result.entity);
			Player* p = dynamic_cast<Player *>(result.entity);

			if (distance < this->radius + result.entity->radius && f == nullptr)
			{
				bool dash = true;
				if (p != nullptr)
					if (p->dashing == false)
						dash = false;
				if (dash == false || result.entity->type == EntityType::Spell)
				{
					XMVECTOR aPos;
					XMVECTOR bPos;
					XMVECTOR bVel;
					aPos = XMVectorSet(this->position.x, this->position.z, 0.f, 0.f);
					bPos = XMVectorSet(result.entity->position.x, result.entity->position.z, 0.f, 0.f);
					bVel = XMVectorSet(result.entity->velocity.x, result.entity->velocity.y, 0.f, 0.f);

					XMVECTOR norm;

					if (this->edge)
						norm = aPos - bPos;
					else
					{
						norm = XMVectorSet(cos(this->angle), sin(this->angle), 0.f, 0.f);
						if (XMVectorGetX(XMVector2Dot(norm, bVel)) < 0)
						{
							norm = -norm;
						}
					}

					norm = XMVector2Normalize(norm);
					bVel = XMVector2Reflect(bVel, norm);

					result.entity->position.x = this->position.x + (XMVectorGetX(-norm) * (this->radius + result.entity->radius + 0.1f));
					result.entity->position.z = this->position.z + (XMVectorGetY(-norm) * (this->radius + result.entity->radius + 0.1f));

					result.entity->velocity.x = XMVectorGetX(bVel);
					result.entity->velocity.y = XMVectorGetY(bVel);
				}


			}
		}
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
			dynamic_cast<Player *>(result.entity)->debuffs.duration = 2.0f;
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

FireWallSpell::FireWallSpell(Player * owner, XMFLOAT3 position, float radius) 
	: Spell(owner, position, { 0,0 }, radius, 10.f)
{
	this->endPos = endPos;
	this->type = EntityType::Wall;
	this->angle = owner->angle;
	this->edge = false;
}

FireWallSpell::~FireWallSpell()
{
}

void FireWallSpell::update(Map * map, float dt)
{
	life -= dt;
	if (life <= 0.f) {
		dead = true;
	}
}

bool FireWallSpell::on_effect(Map * map)
{
	auto nearby = map->get_entities_in_radius(this, radius, [](Entity *e) {
		return true;
	});

	for (auto result : nearby)
	{
		if (result.entity->type != this->type)
		{
			float dx = abs(this->position.x - result.entity->position.x);
			float dz = abs(this->position.z - result.entity->position.z);

			float distance = sqrt(dx * dx + dz * dz);

			WindFartCloudSpell* f = dynamic_cast<WindFartCloudSpell*>(result.entity);
			Player* p = dynamic_cast<Player *>(result.entity);

			if (distance < this->radius + result.entity->radius && f == nullptr)
			{
				bool dash = true;
				if (p != nullptr)
					if (p->dashing == false)
						dash = false;
				if (dash == false || result.entity->type == EntityType::Spell)
				{
					if (pUpgrades[owner->index].choice[1] == 2 && result.entity->type == EntityType::Player)
					{
						Player* victim = dynamic_cast<Player*>(result.entity);
						if (victim != nullptr && victim != owner)
						{
							victim->debuffs.dot = -0.02f;
							victim->debuffs.duration = 1.0f;
						}
					}
					XMVECTOR aPos;
					XMVECTOR bPos;
					XMVECTOR bVel;
					aPos = XMVectorSet(this->position.x, this->position.z, 0.f, 0.f);
					bPos = XMVectorSet(result.entity->position.x, result.entity->position.z, 0.f, 0.f);
					bVel = XMVectorSet(result.entity->velocity.x, result.entity->velocity.y, 0.f, 0.f);

					XMVECTOR norm;

					if (this->edge)
						norm = aPos - bPos;
					else
					{
						norm = XMVectorSet(cos(this->angle), sin(this->angle), 0.f, 0.f);
						if (XMVectorGetX(XMVector2Dot(norm, bVel)) < 0)
						{
							norm = -norm;
						}
					}

					norm = XMVector2Normalize(norm);
					bVel = XMVector2Reflect(bVel, norm);

					result.entity->position.x = this->position.x + (XMVectorGetX(-norm) * (this->radius + result.entity->radius + 0.1f));
					result.entity->position.z = this->position.z + (XMVectorGetY(-norm) * (this->radius + result.entity->radius + 0.1f));

					result.entity->velocity.x = XMVectorGetX(bVel);
					result.entity->velocity.y = XMVectorGetY(bVel);
				}


			}
		}
	}
	return false;
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

EarthWallSpell::EarthWallSpell(Player *owner, XMFLOAT3 position, float radius)
	:Spell(owner, position, { 0,0 }, radius, 10.f)
{
	this->endPos = endPos;
	this->type = EntityType::Wall;
	this->angle = owner->angle;
	this->edge = false;
}

EarthWallSpell::~EarthWallSpell()
{
}

void EarthWallSpell::update(Map * map, float dt)
{
	life -= dt;
	if (life <= 0.f) {
		dead = true;
	}

	if (pUpgrades[owner->index].choice[1] == 2)
	{
		auto nearby = map->get_entities_in_radius(this, gPlayerSpellConstants[owner->index].kEarthWallEffectRadius, [](Entity *e) {
			return true;
		});

		for (auto result : nearby)
		{
			Player* pvictim = nullptr;
			Spell* victim = dynamic_cast<Spell*>(result.entity);

			if (victim != nullptr)
			{
				pvictim = victim->owner;
			}
			if (dynamic_cast<Player *>(result.entity) != owner && pvictim != owner && result.entity->type != EntityType::Wall)
			{
				float falloff = (abs(gPlayerSpellConstants[owner->index].kEarthWallEffectRadius - result.distance)
					/ (gPlayerSpellConstants[owner->index].kEarthWallEffectRadius))
					* (gPlayerSpellConstants[owner->index].kEarthWallEffectFalloff);

				result.entity->velocity.x -= cos(result.angle) * ((gPlayerSpellConstants[owner->index].kEarthWallStrength / (gSpellConstants.kEarthWallNrOfPillars + gPlayerSpellConstants[owner->index].kEarthWallNrOfPillars)) * falloff);
				result.entity->velocity.y -= sin(result.angle) * ((gPlayerSpellConstants[owner->index].kEarthWallStrength / (gSpellConstants.kEarthWallNrOfPillars + gPlayerSpellConstants[owner->index].kEarthWallNrOfPillars)) * falloff);
			}
		}
	}
}

bool EarthWallSpell::on_effect(Map *map) { //made so earthwall has its own class for upgrades
	auto nearby = map->get_entities_in_radius(this, radius, [](Entity *e) {
		return true;
	});

	for (auto result : nearby)
	{
		if (result.entity->type != this->type)
		{
			float dx = abs(this->position.x - result.entity->position.x);
			float dz = abs(this->position.z - result.entity->position.z);

			float distance = sqrt(dx * dx + dz * dz);

			WindFartCloudSpell* f = dynamic_cast<WindFartCloudSpell*>(result.entity);
			Player* p = dynamic_cast<Player *>(result.entity);

			if (distance < this->radius + result.entity->radius && f == nullptr)
			{
				bool dash = true;
				if (p != nullptr)
					if (p->dashing == false)
						dash = false;
				if (dash == false || result.entity->type == EntityType::Spell)
				{
					if (pUpgrades[owner->index].choice[1] == 1 && result.entity->type == EntityType::Player)
					{
						Player* victim = dynamic_cast<Player*>(result.entity);
						if (victim != nullptr && victim != owner)
						{
							victim->debuffs.dot = -0.2f;
							victim->debuffs.duration = 0.1f;
						}
					}

					Player* pvictim = nullptr;
					Spell* victim = dynamic_cast<Spell*>(result.entity);

					if (victim != nullptr)
					{
						pvictim = victim->owner;
					}

					if (pUpgrades[owner->index].choice[1] != 2 || dynamic_cast<Player *>(result.entity) == owner || pvictim == owner)
					{
						XMVECTOR aPos;
						XMVECTOR bPos;
						XMVECTOR bVel;
						aPos = XMVectorSet(this->position.x, this->position.z, 0.f, 0.f);
						bPos = XMVectorSet(result.entity->position.x, result.entity->position.z, 0.f, 0.f);
						bVel = XMVectorSet(result.entity->velocity.x, result.entity->velocity.y, 0.f, 0.f);

						XMVECTOR norm;

						if (this->edge)
							norm = aPos - bPos;
						else
						{
							norm = XMVectorSet(cos(this->angle), sin(this->angle), 0.f, 0.f);
							if (XMVectorGetX(XMVector2Dot(norm, bVel)) < 0)
							{
								norm = -norm;
							}
						}

						norm = XMVector2Normalize(norm);
						bVel = XMVector2Reflect(bVel, norm);

						result.entity->position.x = this->position.x + (XMVectorGetX(-norm) * (this->radius + result.entity->radius + 0.1f));
						result.entity->position.z = this->position.z + (XMVectorGetY(-norm) * (this->radius + result.entity->radius + 0.1f));

						result.entity->velocity.x = XMVectorGetX(bVel);
						result.entity->velocity.y = XMVectorGetY(bVel);
					}
				}


			}
		}
	}
	return false;
};

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
	if (map->playerElemnts[this->owner->index] == 4)
	{
		dynamic_cast<WaterElement*>(this->owner->element)->active_projectile = nullptr;
	}
	
	return true;
}

WindFartCloudSpell::WindFartCloudSpell(Player * owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(owner, position, velocity, radius, 4.5f)
{
}

WindFartCloudSpell::~WindFartCloudSpell()
{
}

void WindFartCloudSpell::update(Map * map, float dt)
{
	Spell::update(map, dt);
}

bool WindFartCloudSpell::on_effect(Map * map)
{
	auto nearby = map->get_entities_in_radius(this, radius, [](Entity *e) {
		return e->type == EntityType::Player;
	});

	for (auto result : nearby) {
		if (result.entity != owner)
		{
			dynamic_cast<Player*>(result.entity)->debuffs.dot = -0.2f;
			dynamic_cast<Player*>(result.entity)->debuffs.duration = 0.1f;
		}
	}

	return false;
}

WindBeaconSpell::WindBeaconSpell(Player * owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(owner, position, velocity, radius, 4.5f), lifetime(0.0f), stomped(false)
{
}

WindBeaconSpell::~WindBeaconSpell()
{
}

void WindBeaconSpell::update(Map * map, float dt)
{
	if (lifetime <= 0.0f)
	{
		stomped = true;
		//saves nearby players in a vector
		auto nearby = map->get_entities_in_radius(this, gSpellConstants.kWindStompDistance + gPlayerSpellConstants[owner->index].kWindStompDistance, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby) { //moves all nearby players
			if (result.entity != owner)
			{
				result.entity->velocity.x += cos(result.angle) * (gSpellConstants.kWindStompStrength + gPlayerSpellConstants[owner->index].kWindStompStrength) * abs((gSpellConstants.kWindStompDistance + gPlayerSpellConstants[owner->index].kWindStompDistance + gSpellConstants.kWindStompStrengthFalloff + gPlayerSpellConstants[owner->index].kWindStompStrengthFalloff) - result.distance);
				result.entity->velocity.y += sin(result.angle) * (gSpellConstants.kWindStompStrength + gPlayerSpellConstants[owner->index].kWindStompStrength) * abs((gSpellConstants.kWindStompDistance + gPlayerSpellConstants[owner->index].kWindStompDistance + gSpellConstants.kWindStompStrengthFalloff + gPlayerSpellConstants[owner->index].kWindStompStrengthFalloff) - result.distance);
			}
		}

		lifetime = gSpellConstants.kWindStompCooldown + gPlayerSpellConstants[owner->index].kWindStompCooldown;
		map->sounds.play(spellSounds::arcaneStomp, 0.0f, 80.0f);
	}
	else
	{
		lifetime -= dt;
	}
}

bool WindBeaconSpell::on_effect(Map * map)
{
	return false;
}

bool WindBeaconSpell::hasStomped()
{
	return this->stomped;
}

void WindBeaconSpell::endStomped()
{
	this->stomped = false;
}

WaterIcePatch::WaterIcePatch(Player * owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(owner, position, {0.0f, 0.0f}, radius, 6.3f)
{

}

WaterIcePatch::~WaterIcePatch()
{
}

void WaterIcePatch::update(Map * map, float dt)
{
	life -= dt;
	if (life <= 0.f) {
		dead = true;
	}
}

bool WaterIcePatch::on_effect(Map * map)
{
	auto nearby = map->get_entities_in_radius(this, radius, [](Entity *e) {
		return e->type == EntityType::Player;
	});

	for (auto result : nearby) {
		if (dynamic_cast<Player*>(result.entity) != this->owner)
		{
			dynamic_cast<Player*>(result.entity)->debuffs.speed = this->dSpeed;
			dynamic_cast<Player*>(result.entity)->debuffs.friction = this->dFriction;
			dynamic_cast<Player*>(result.entity)->debuffs.duration = 0.4f;
		}
		
	}
	return false;
}

FirePathSpell::FirePathSpell(Player * owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(owner, position, { 0.0f, 0.0f }, radius, 6.3f)
{
	this->pEmitter.particleType = 1;
	this->pEmitter.position = position;
	this->pEmitter.randomVector = DirectX::XMFLOAT4(position.x, position.y, position.z, 1.0f);
}

FirePathSpell::~FirePathSpell()
{
}

void FirePathSpell::update(Map * map, float dt)
{
	life -= dt;
	if (life <= 0.f) {
		dead = true;
	}
}

bool FirePathSpell::on_effect(Map * map)
{
	auto nearby = map->get_entities_in_radius(this, radius, [](Entity *e) {
		return e->type == EntityType::Player;
	});

	for (auto result : nearby) {
		if (dynamic_cast<Player*>(result.entity) != this->owner)
		{
			dynamic_cast<Player*>(result.entity)->debuffs.dot = this->dot;
			dynamic_cast<Player*>(result.entity)->debuffs.duration = 0.32f;
		}

	}
	return false;
}
