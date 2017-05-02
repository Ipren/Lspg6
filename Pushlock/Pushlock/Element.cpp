#include "Element.h"

#include "Spell.h"
#include "Player.h"
#include "Constants.h"
#include "Upgrades.h"

void ArcaneElement::projectile(Player *player, Map *map)
{
	if (cooldown[0] <= 0.f) {
		if (pUpgrades[player->index].choice[0] == 1)
		{
			if (gPlayerSpellConstants[player->index].kArcaneProjectileStrength > -15.0f)
			{
				
				gPlayerSpellConstants[player->index].kArcaneProjectileStrength = -15.0f;
				gPlayerSpellConstants[player->index].kArcaneProjectileCooldown = 1.2f;
			}

			auto position = player->position;
			auto angle = player->angle;
			auto radius = player->radius;
			XMFLOAT2 v = {0.0f, 0.0f};

			ArcaneProjectileSpell *spell = new ArcaneProjectileSpell(player,
			{
				position.x + cos(angle) * (radius + 0.4f),
				0,
				position.z + sin(angle) * (radius + 0.4f)
			},
			{ cos(angle) * (gSpellConstants.kArcaneProjectileSpeed + gPlayerSpellConstants[player->index].kArcaneProjectileSpeed),
				sin(angle) * (gSpellConstants.kArcaneProjectileSpeed + gPlayerSpellConstants[player->index].kArcaneProjectileSpeed) },
				0.1f
			);
			map->add_entity(spell);

			for (size_t i = 1; i < 3; i++)
			{
				v = { cos(angle) * (gSpellConstants.kArcaneProjectileSpeed + gPlayerSpellConstants[player->index].kArcaneProjectileSpeed), sin(angle) * (gSpellConstants.kArcaneProjectileSpeed + gPlayerSpellConstants[player->index].kArcaneProjectileSpeed) };
				if (i % 2 != 0)
				{
					v.x = v.x * cos(i * 20 * XM_PI / 180) - v.y * sin(i * 20 * XM_PI / 180);
					v.y = v.x * sin(i * 20 * XM_PI / 180) + v.y * cos(i * 20 * XM_PI / 180);
					spell = new ArcaneProjectileSpell(player,
					{
						position.x + cos(angle) * (radius + 0.4f),
						0,
						position.z + sin(angle) * (radius + 0.4f)
					},
						v,
						0.1f
					);
					map->add_entity(spell);
				}
				else
				{
					v.x = v.x * cos((i-1) * 15 * XM_PI / 180) + v.y * sin((i - 1) * 15 * XM_PI / 180);
					v.y = -v.x * sin((i - 1) * 15 * XM_PI / 180) + v.y * cos((i - 1) * 15 * XM_PI / 180);
					spell = new ArcaneProjectileSpell(player,
					{
						position.x + cos(angle) * (radius + 0.4f),
						0,
						position.z + sin(angle) * (radius + 0.4f)
					},
						v,
						0.1f
					);
					map->add_entity(spell);
				}
				
			}
		}
		else 
		{
			auto position = player->position;
			auto angle = player->angle;
			auto radius = player->radius;

			ArcaneProjectileSpell *spell = new ArcaneProjectileSpell(player,
			{
				position.x + cos(angle) * (radius + 0.4f),
				0,
				position.z + sin(angle) * (radius + 0.4f)
			},
			{ cos(angle) * (gSpellConstants.kArcaneProjectileSpeed + gPlayerSpellConstants[player->index].kArcaneProjectileSpeed),
				sin(angle) * (gSpellConstants.kArcaneProjectileSpeed + gPlayerSpellConstants[player->index].kArcaneProjectileSpeed) },
				0.1f
			);

			map->add_entity(spell);
		}
		cooldown[0] = gSpellConstants.kArcaneProjectileCooldown + gPlayerSpellConstants[player->index].kArcaneProjectileCooldown;
		map->sounds.play(spellSounds::arcaneProjectile, 0.0f, 50.0f);
	}
}

void ArcaneElement::stomp(Player *player, Map *map)
{
	if (cooldown[2] <= 0.f) {

		player->stomped = true;
		//saves nearby players in a vector
		auto nearby = map->get_entities_in_radius(player, gSpellConstants.kArcaneStompDistance + gPlayerSpellConstants[player->index].kArcaneStompDistance, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby) { //moves all nearby players
			result.entity->velocity.x += cos(result.angle) * (gSpellConstants.kArcaneStompStrength + gPlayerSpellConstants[player->index].kArcaneStompStrength) * abs((gSpellConstants.kArcaneStompDistance + gPlayerSpellConstants[player->index].kArcaneStompDistance + gSpellConstants.kArcaneStompStrengthFalloff + gPlayerSpellConstants[player->index].kArcaneStompStrengthFalloff) - result.distance);
			result.entity->velocity.y += sin(result.angle) * (gSpellConstants.kArcaneStompStrength + gPlayerSpellConstants[player->index].kArcaneStompStrength) * abs((gSpellConstants.kArcaneStompDistance + gPlayerSpellConstants[player->index].kArcaneStompDistance + gSpellConstants.kArcaneStompStrengthFalloff + gPlayerSpellConstants[player->index].kArcaneStompStrengthFalloff) - result.distance);
		}

		cooldown[2] = gSpellConstants.kArcaneStompCooldown + gPlayerSpellConstants[player->index].kArcaneStompCooldown;
		map->sounds.play(spellSounds::arcaneStomp, 0.0f, 80.0f);
		
	}
}

void ArcaneElement::wall(Player *player, Map *map)
{
	if (cooldown[3] <= 0.f) {
		XMFLOAT3 position = player->position;
		float angle = player->angle;
		float radius = player->radius;

		XMVECTOR pos = {
			position.x + cos(angle) * (radius + 1.5f),
			0,
			position.z + sin(angle) * (radius + 1.5f)
		};

		XMVECTOR dist = pos - XMLoadFloat3(&position);
		XMVECTOR n = XMVector3Cross(dist, { 0, 1, 0 });

		int nrOfPillars = gSpellConstants.kArcaneWallNrOfPillars + gPlayerSpellConstants[player->index].kArcaneWallNrOfPillars;

		for (int i = 0; i < nrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - (gSpellConstants.kFireWallNrOfPillars + gPlayerSpellConstants[player->index].kFireWallNrOfPillars) / 2.f) *
				(gSpellConstants.kFireWallPillarDistance + gPlayerSpellConstants[player->index].kFireWallPillarDistance) / 2.f + pos);

			Entity* e = map->add_entity(new ArcaneWallSpell(player, p, 
				gSpellConstants.kArcaneWallPillarRadius + gPlayerSpellConstants[player->index].kArcaneWallPillarRadius));
			if (i == 0 || i == nrOfPillars-1)
			{
				ArcaneWallSpell* w = dynamic_cast<ArcaneWallSpell*>(e);
				w->edge = true;
			}
		}


		cooldown[3] = gSpellConstants.kArcaneWallCooldown + gPlayerSpellConstants[player->index].kArcaneWallCooldown;
		map->sounds.play(spellSounds::earthWall, 0.0f, 100.0f);
	}

}

void ArcaneElement::dash(Player * player, Map * map)
{
	if (cooldown[1] <= 0.f) {
		auto index = player->index;
		XMFLOAT2 leftVector = gGamepads[index]->get_left_thumb();

		if (pUpgrades[player->index].choice[0] == 2)
		{

			if (leftVector.x != 0.f && leftVector.y != 0.f)//if it is not: dash to where you are walking
			{
				float left_angle = gGamepads[index]->get_left_thumb_angle();
				player->position.x += cos(left_angle) * 5.0f;
				player->position.z += sin(left_angle) * 5.0f;
			}
			else
			{
				player->position.x += cos(player->angle) * 5.0f;
				player->position.z += sin(player->angle) * 5.0f;
			}
			player->dashing = true;
			player->dashTime = 0.0f;
		}
		else
		{
			//check if left stick is centered
			if (leftVector.x != 0.f && leftVector.y != 0.f)//if it is not: dash to where you are walking
			{
				float left_angle = gGamepads[index]->get_left_thumb_angle();
				player->velocity.x += cos(left_angle) * (gSpellConstants.kArcaneDashSpeed + gPlayerSpellConstants[player->index].kArcaneDashSpeed);
				player->velocity.y += sin(left_angle) * (gSpellConstants.kArcaneDashSpeed + gPlayerSpellConstants[player->index].kArcaneDashSpeed);
			}
			else //if it is centered: dash to where you are looking
			{
				player->velocity.x += cos(player->angle) * (gSpellConstants.kArcaneDashSpeed + gPlayerSpellConstants[player->index].kArcaneDashSpeed);
				player->velocity.y += sin(player->angle) * (gSpellConstants.kArcaneDashSpeed + gPlayerSpellConstants[player->index].kArcaneDashSpeed);
			}
		}
		cooldown[1] = gSpellConstants.kArcaneDashCooldown + gPlayerSpellConstants[player->index].kArcaneDashCooldown;
		map->sounds.play(spellSounds::windDash, 0.0f, 50.0f);
	}
}

FireElement::FireElement()
	: active_projectile(nullptr)
{
	this->startHealth = 10.f;

}

void FireElement::projectile(Player * player, Map * map)
{
	if (cooldown[0] <= 0.f || active_projectile != nullptr)
	{
		if (active_projectile == nullptr) {
			 {
				auto position = player->position;
				auto angle = player->angle;
				auto radius = player->radius;

				FireProjectileSpell *spell = new FireProjectileSpell(player,
				{
					position.x + cos(angle) * (radius + 0.4f),
					0,
					position.z + sin(angle) * (radius + 0.4f)
				},
				{ cos(angle) * (gSpellConstants.kFireProjectileSpeed + gPlayerSpellConstants[player->index].kFireProjectileSpeed),
					sin(angle) * (gSpellConstants.kFireProjectileSpeed + gPlayerSpellConstants[player->index].kFireProjectileSpeed) },
					0.1f
				);

				map->add_entity(spell);
				active_projectile = spell;
				map->sounds.play(spellSounds::fireProjectile, 0.0f, 50.0f);
				cooldown[0] = gSpellConstants.kFireProjectileCooldown + gPlayerSpellConstants[player->index].kFireProjectileCooldown;
			}
		}
		else {
			if (active_projectile->dead != true)
			{
				player->blowUp = true;
				if (active_projectile->on_effect(map)) {
					//cooldown[0] = gSpellConstants.kFireProjectileCooldown + gPlayerSpellConstants[player->index].kFireProjectileCooldown;
					{
						
						active_projectile->dead = true;

					}
				}
			}
			else
			{
				active_projectile->dead = true;
				active_projectile = nullptr;
			}

		}
	}
}

void FireElement::stomp(Player * player, Map * map)
{
	if (cooldown[2] <= 0.f) {

		player->stomped = true;

		//saves nearby players in a vector
		auto nearby = map->get_entities_in_radius(player, gSpellConstants.kFireStompDistance + gPlayerSpellConstants[player->index].kFireStompDistance, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby) { //moves all nearby players
			result.entity->velocity.x += cos(result.angle) * (gSpellConstants.kFireStompStrength + gPlayerSpellConstants[player->index].kFireStompStrength) * abs((gSpellConstants.kFireStompDistance + gPlayerSpellConstants[player->index].kFireStompDistance + gSpellConstants.kFireStompStrengthFalloff + gPlayerSpellConstants[player->index].kFireStompStrengthFalloff) - result.distance);
			result.entity->velocity.y += sin(result.angle) * (gSpellConstants.kFireStompStrength + gPlayerSpellConstants[player->index].kFireStompStrength) * abs((gSpellConstants.kFireStompDistance + gPlayerSpellConstants[player->index].kFireStompDistance + gSpellConstants.kFireStompStrengthFalloff + gPlayerSpellConstants[player->index].kFireStompStrengthFalloff) - result.distance);
			
			if (pUpgrades[player->index].choice[0] == 2)
			{
				player->element->cooldown[1] = 0.0f;
			}
		}

		cooldown[2] = gSpellConstants.kFireStompCooldown + gPlayerSpellConstants[player->index].kFireStompCooldown;
		map->sounds.play(spellSounds::arcaneStomp, 0.0f, 80.0f);
	}
}

void FireElement::wall(Player * player, Map * map)
{
	if (cooldown[3] <= 0.f) {
		auto position = player->position;
		auto angle = player->angle;
		auto radius = player->radius;

		XMVECTOR pos = {
			position.x + cos(angle) * (radius + 1.5f),
			0,
			position.z + sin(angle) * (radius + 1.5f)
		};

		XMVECTOR dist = pos - XMLoadFloat3(&position);
		XMVECTOR n = XMVector3Cross(dist, { 0, 1, 0 });

		int nrOfPillars = gSpellConstants.kArcaneWallNrOfPillars + gPlayerSpellConstants[player->index].kArcaneWallNrOfPillars;

		for (int i = 0; i < nrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - (gSpellConstants.kFireWallNrOfPillars + gPlayerSpellConstants[player->index].kFireWallNrOfPillars) / 2.f) *
				(gSpellConstants.kFireWallPillarDistance + gPlayerSpellConstants[player->index].kFireWallPillarDistance) / 2.f + pos);

			Entity* e = map->add_entity(new ArcaneWallSpell(player, p,
				gSpellConstants.kArcaneWallPillarRadius + gPlayerSpellConstants[player->index].kArcaneWallPillarRadius));
			if (i == 0 || i == nrOfPillars - 1)
			{
				ArcaneWallSpell* w = dynamic_cast<ArcaneWallSpell*>(e);
				w->edge = true;
			}
		}

		cooldown[3] = gSpellConstants.kFireWallCooldown + gPlayerSpellConstants[player->index].kFireWallCooldown;
		map->sounds.play(spellSounds::earthWall, 0.0f, 100.0f);
	}
}

void FireElement::dash(Player * player, Map * map)
{
	if (cooldown[1] <= 0.f) {
		auto index = player->index;
		XMFLOAT2 leftVector = gGamepads[index]->get_left_thumb();

		//check if left stick is centered
		if (leftVector.x != 0.f && leftVector.y != 0.f)//if it is not: dash to where you are walking
		{
			float left_angle = gGamepads[index]->get_left_thumb_angle();
			player->velocity.x += cos(left_angle) * (gSpellConstants.kFireDashSpeed + gPlayerSpellConstants[player->index].kFireDashSpeed);
			player->velocity.y += sin(left_angle) * (gSpellConstants.kFireDashSpeed + gPlayerSpellConstants[player->index].kFireDashSpeed);
		}
		else //if it is centered: dash to where you are looking
		{
			player->velocity.x += cos(player->angle) * (gSpellConstants.kFireDashSpeed + gPlayerSpellConstants[player->index].kFireDashSpeed);
			player->velocity.y += sin(player->angle) * (gSpellConstants.kFireDashSpeed + gPlayerSpellConstants[player->index].kFireDashSpeed);
		}

		cooldown[1] = gSpellConstants.kFireDashCooldown + gPlayerSpellConstants[player->index].kFireDashCooldown;
		map->sounds.play(spellSounds::windDash, 0.0f, 50.0f);
	}
}

void WindElement::projectile(Player * player, Map * map)
{
	if (cooldown[0] <= 0.f) {
		auto position = player->position;
		auto angle = player->angle;
		auto radius = player->radius;

		WindProjectileSpell *spell = new WindProjectileSpell(player,
		{
			position.x + cos(angle) * (radius + 0.4f),
			0,
			position.z + sin(angle) * (radius + 0.4f)
		},
		{ cos(angle) * (gSpellConstants.kWindProjectileSpeed + gPlayerSpellConstants[player->index].kWindProjectileSpeed), sin(angle) * (gSpellConstants.kWindProjectileSpeed + gPlayerSpellConstants[player->index].kWindProjectileSpeed) },
			0.1f
		);

		map->add_entity(spell);
		cooldown[0] = gSpellConstants.kWindProjectileCooldown + gPlayerSpellConstants[player->index].kWindProjectileCooldown;
		map->sounds.play(spellSounds::windDash, 0.0f, 2.0f);
	}
}

void WindElement::stomp(Player * player, Map * map)
{
	if (cooldown[2] <= 0.f) {

		player->stomped = true;
		//saves nearby players in a vector
		auto nearby = map->get_entities_in_radius(player, gSpellConstants.kWindStompDistance + gPlayerSpellConstants[player->index].kWindStompDistance, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby) { //moves all nearby players
			result.entity->velocity.x += cos(result.angle) * (gSpellConstants.kWindStompStrength + gPlayerSpellConstants[player->index].kWindStompStrength) * abs((gSpellConstants.kWindStompDistance + gPlayerSpellConstants[player->index].kWindStompDistance + gSpellConstants.kWindStompStrengthFalloff + gPlayerSpellConstants[player->index].kWindStompStrengthFalloff) - result.distance);
			result.entity->velocity.y += sin(result.angle) * (gSpellConstants.kWindStompStrength + gPlayerSpellConstants[player->index].kWindStompStrength) * abs((gSpellConstants.kWindStompDistance + gPlayerSpellConstants[player->index].kWindStompDistance + gSpellConstants.kWindStompStrengthFalloff + gPlayerSpellConstants[player->index].kWindStompStrengthFalloff) - result.distance);
		}

		cooldown[2] = gSpellConstants.kWindStompCooldown + gPlayerSpellConstants[player->index].kWindStompCooldown;
		map->sounds.play(spellSounds::arcaneStomp, 0.0f, 80.0f);
	}
}

void WindElement::wall(Player * player, Map * map)
{
	if (cooldown[3] <= 0.f) {
		auto position = player->position;
		auto angle = player->angle;
		auto radius = player->radius;

		XMVECTOR pos = {
			position.x + cos(angle) * (radius + 1.5f),
			0,
			position.z + sin(angle) * (radius + 1.5f)
		};

		XMVECTOR dist = pos - XMLoadFloat3(&position);
		XMVECTOR n = XMVector3Cross(dist, { 0, 1, 0 });

		int nrOfPillars = gSpellConstants.kArcaneWallNrOfPillars + gPlayerSpellConstants[player->index].kArcaneWallNrOfPillars;

		for (int i = 0; i < nrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - (gSpellConstants.kFireWallNrOfPillars + gPlayerSpellConstants[player->index].kFireWallNrOfPillars) / 2.f) *
				(gSpellConstants.kFireWallPillarDistance + gPlayerSpellConstants[player->index].kFireWallPillarDistance) / 2.f + pos);

			Entity* e = map->add_entity(new ArcaneWallSpell(player, p,
				gSpellConstants.kArcaneWallPillarRadius + gPlayerSpellConstants[player->index].kArcaneWallPillarRadius));
			if (i == 0 || i == nrOfPillars - 1)
			{
				ArcaneWallSpell* w = dynamic_cast<ArcaneWallSpell*>(e);
				w->edge = true;
			}
		}

		cooldown[3] = gSpellConstants.kWindWallCooldown  + gPlayerSpellConstants[player->index].kWindWallCooldown;
		map->sounds.play(spellSounds::earthWall, 0.0f, 100.0f);
	}
}

void WindElement::dash(Player * player, Map * map)
{
	if (cooldown[1] <= 0.f) {
		auto index = player->index;
		XMFLOAT2 leftVector = gGamepads[index]->get_left_thumb();

		//check if left stick is centered
		if (leftVector.x != 0.f && leftVector.y != 0.f)//if it is not: dash to where you are walking
		{
			float left_angle = gGamepads[index]->get_left_thumb_angle();
			player->velocity.x += cos(left_angle) * (gSpellConstants.kWindDashSpeed + gPlayerSpellConstants[player->index].kWindDashSpeed);
			player->velocity.y += sin(left_angle) * (gSpellConstants.kWindDashSpeed + gPlayerSpellConstants[player->index].kWindDashSpeed);
		}
		else //if it is centered: dash to where you are looking
		{
			player->velocity.x += cos(player->angle) * (gSpellConstants.kWindDashSpeed + gPlayerSpellConstants[player->index].kWindDashSpeed);
			player->velocity.y += sin(player->angle) * (gSpellConstants.kWindDashSpeed + gPlayerSpellConstants[player->index].kWindDashSpeed);
		}
		if (pUpgrades[player->index].choice[0] == 2)
		{
			player->dashing = true;
			player->dashTime = 0.0f;
		}

		cooldown[1] = gSpellConstants.kWindDashCooldown + gPlayerSpellConstants[player->index].kWindDashCooldown;
		map->sounds.play(spellSounds::windDash, 0.0f, 50.0f);
	}
}

void EarthElement::projectile(Player * player, Map * map)
{
	if (cooldown[0] <= 0.f) {
		auto position = player->position;
		auto angle = player->angle;
		auto radius = player->radius;

		EarthProjectileSpell *spell = new EarthProjectileSpell(player,
		{
			position.x + cos(angle) * (radius + 0.4f),
			0,
			position.z + sin(angle) * (radius + 0.4f)
		},
		{ cos(angle) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed), sin(angle) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed) },
			0.1f
		);

		map->add_entity(spell);
		cooldown[0] = gSpellConstants.kEarthProjectileCooldown + gPlayerSpellConstants[player->index].kEarthProjectileSpeed;
		map->sounds.play(spellSounds::earthProjectile, 0.0f, 100.0f);

	}
}

void EarthElement::stomp(Player * player, Map * map)
{
	if (cooldown[2] <= 0.f) {
		if (pUpgrades[player->index].choice[0] == 1)
		{
			auto position = player->position;
			auto angle = player->angle;
			auto radius = player->radius;
			Entity *e = nullptr;
			for (size_t i = 1; i < 5; i++)
			{
				e = new Entity(EntityType::emitter, { position.x + cos(angle) * (radius + 0.4f + (float)i), 0.0f, position.z + sin(angle) * (radius + 0.4f + (float)i) }, { 0.0f, 0.0f }, radius);
				map->add_entity(e);
				auto nearby = map->get_entities_in_radius(e, gSpellConstants.kEarthStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance, [](Entity *e) {
					return e->type == EntityType::Player;
				});

				for (auto result : nearby) { //moves all nearby players
					if (dynamic_cast<Player*>(result.entity) != player)
					{
						result.entity->velocity.x += 0.35f * cos(result.angle) * (gSpellConstants.kEarthStompStrength + gPlayerSpellConstants[player->index].kEarthStompStrength) * abs((gSpellConstants.kEarthStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance + gSpellConstants.kEarthStompStrengthFalloff + gPlayerSpellConstants[player->index].kEarthStompStrengthFalloff) - result.distance);
						result.entity->velocity.y += 0.35f * sin(result.angle) * (gSpellConstants.kEarthStompStrength + gPlayerSpellConstants[player->index].kEarthStompStrength) * abs((gSpellConstants.kEarthStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance + gSpellConstants.kEarthStompStrengthFalloff + gPlayerSpellConstants[player->index].kEarthStompStrengthFalloff) - result.distance);
					}
					
				}
			}
		}
		else if(pUpgrades[player->index].choice[0] == 2)
		{
			player->stomped = true;
			//saves nearby players in a vector
			auto nearby = map->get_entities_in_radius(player, gSpellConstants.kEarthStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance, [](Entity *e) {
				return e->type == EntityType::Player;
			});

			for (auto result : nearby) { //moves all nearby players
				result.entity->velocity.x += cos(result.angle) * (gSpellConstants.kEarthStompStrength + gPlayerSpellConstants[player->index].kEarthStompStrength) * abs((gSpellConstants.kEarthStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance + gSpellConstants.kEarthStompStrengthFalloff + gPlayerSpellConstants[player->index].kEarthStompStrengthFalloff) - result.distance);
				result.entity->velocity.y += sin(result.angle) * (gSpellConstants.kEarthStompStrength + gPlayerSpellConstants[player->index].kEarthStompStrength) * abs((gSpellConstants.kEarthStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance + gSpellConstants.kEarthStompStrengthFalloff + gPlayerSpellConstants[player->index].kEarthStompStrengthFalloff) - result.distance);
			}
			auto position = player->position;
			auto radius = player->radius;

			WaterProjectileSpell *spell = new WaterProjectileSpell(player,
			{
				position.x + cos((float)0) * (radius + 0.4f),
				0,
				position.z + sin((float)0) * (radius + 0.4f)
			},
			{ cos((float)0) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed),
				sin((float)0) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed) },
				0.1f
			);
			spell->pEmitter.particleType = -1;
			map->add_entity(spell);

			spell = new WaterProjectileSpell(player,
			{
				position.x + cos((float)2.45f) * (radius + 0.4f),
				0,
				position.z + sin((float)2.45f) * (radius + 0.4f)
			},
			{ cos((float)2.45f) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed),
				sin((float)2.45f) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed) },
				0.1f
			);
			spell->pEmitter.particleType = -1;
			map->add_entity(spell);
			spell = new WaterProjectileSpell(player,
			{
				position.x + cos((float)0.9f) * (radius + 0.4f),
				0,
				position.z + sin((float)0.9f) * (radius + 0.4f)
			},
			{ cos((float)0.9f) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed),
				sin((float)0.9f) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed) },
				0.1f
			);
			spell->pEmitter.particleType = -1;
			map->add_entity(spell);
			

			spell = new WaterProjectileSpell(player,
			{
				position.x + cos((float)4.1f) * (radius + 0.4f),
				0,
				position.z + sin((float)4.1f) * (radius + 0.4f)
			},
			{ cos((float)4.1f) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed),
				sin((float)4.1f) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed) },
				0.1f
			);
			spell->pEmitter.particleType = -1;
			map->add_entity(spell);

			spell = new WaterProjectileSpell(player,
			{
				position.x + cos((float)5.6f) * (radius + 0.4f),
				0,
				position.z + sin((float)5.6f) * (radius + 0.4f)
			},
			{ cos((float)5.6f) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed),
				sin((float)5.6f) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed) },
				0.1f
			);
			spell->pEmitter.particleType = -1;
			map->add_entity(spell);

			spell = new WaterProjectileSpell(player,
			{
				position.x + cos((float)(XM_PI/2)) * (radius + 0.4f),
				0,
				position.z + sin((float)(XM_PI / 2)) * (radius + 0.4f)
			},
			{ cos((float)XM_PI / 2) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed),
				sin((float)XM_PI / 2) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed) },
				0.1f
			);
			spell->pEmitter.particleType = -1;
			map->add_entity(spell);
			spell = new WaterProjectileSpell(player,
			{
				position.x + cos((float)XM_PI) * (radius + 0.4f),
				0,
				position.z + sin((float)XM_PI) * (radius + 0.4f)
			},
			{ cos((float)XM_PI) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed),
				sin((float)XM_PI) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed) },
				0.1f
			);
			spell->pEmitter.particleType = -1;
			map->add_entity(spell);
			spell = new WaterProjectileSpell(player,
			{
				position.x + cos(270 * XM_PI / 180) * (radius + 0.4f),
				0,
				position.z + sin(270 * XM_PI / 180) * (radius + 0.4f)
			},
			{ cos(270 * XM_PI / 180) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed),
				sin(270 * XM_PI / 180) * (gSpellConstants.kEarthProjectileSpeed + gPlayerSpellConstants[player->index].kEarthProjectileSpeed) },
				0.1f
			);
			spell->pEmitter.particleType = -1;
			map->add_entity(spell);
		
		}
		else
		{
			player->stomped = true;
			//saves nearby players in a vector
			auto nearby = map->get_entities_in_radius(player, gSpellConstants.kEarthStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance, [](Entity *e) {
				return e->type == EntityType::Player;
			});

			for (auto result : nearby) { //moves all nearby players
				result.entity->velocity.x += cos(result.angle) * (gSpellConstants.kEarthStompStrength + gPlayerSpellConstants[player->index].kEarthStompStrength) * abs((gSpellConstants.kEarthStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance + gSpellConstants.kEarthStompStrengthFalloff + gPlayerSpellConstants[player->index].kEarthStompStrengthFalloff) - result.distance);
				result.entity->velocity.y += sin(result.angle) * (gSpellConstants.kEarthStompStrength + gPlayerSpellConstants[player->index].kEarthStompStrength) * abs((gSpellConstants.kEarthStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance + gSpellConstants.kEarthStompStrengthFalloff + gPlayerSpellConstants[player->index].kEarthStompStrengthFalloff) - result.distance);
			}

			
		}
		cooldown[2] = gSpellConstants.kEarthStompCooldown + gPlayerSpellConstants[player->index].kEarthStompCooldown;
		map->sounds.play(spellSounds::arcaneStomp, 0.0f, 80.0f);
	}
}

void EarthElement::wall(Player * player, Map * map)
{
	if (cooldown[3] <= 0.f) {
		auto position = player->position;
		auto angle = player->angle;
		auto radius = player->radius;

		XMVECTOR pos = {
			position.x + cos(angle) * (radius + 1.5f),
			0,
			position.z + sin(angle) * (radius + 1.5f)
		};

		XMVECTOR dist = pos - XMLoadFloat3(&position);
		XMVECTOR n = XMVector3Cross(dist, { 0, 1, 0 });

		int nrOfPillars = gSpellConstants.kArcaneWallNrOfPillars + gPlayerSpellConstants[player->index].kArcaneWallNrOfPillars;

		for (int i = 0; i < nrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - (gSpellConstants.kFireWallNrOfPillars + gPlayerSpellConstants[player->index].kFireWallNrOfPillars) / 2.f) *
				(gSpellConstants.kFireWallPillarDistance + gPlayerSpellConstants[player->index].kFireWallPillarDistance) / 2.f + pos);

			Entity* e = map->add_entity(new ArcaneWallSpell(player, p,
				gSpellConstants.kArcaneWallPillarRadius + gPlayerSpellConstants[player->index].kArcaneWallPillarRadius));
			if (i == 0 || i == nrOfPillars - 1)
			{
				ArcaneWallSpell* w = dynamic_cast<ArcaneWallSpell*>(e);
				w->edge = true;
			}
		}

		cooldown[3] = gSpellConstants.kEarthWallCooldown + gPlayerSpellConstants[player->index].kEarthWallCooldown;
		map->sounds.play(spellSounds::earthWall, 0.0f, 100.0f);
	}
}

void EarthElement::dash(Player * player, Map * map)
{
	if (cooldown[1] <= 0.f) {
		auto index = player->index;
		XMFLOAT2 leftVector = gGamepads[index]->get_left_thumb();

		//check if left stick is centered
		if (leftVector.x != 0.f && leftVector.y != 0.f)//if it is not: dash to where you are walking
		{
			float left_angle = gGamepads[index]->get_left_thumb_angle();
			player->velocity.x += cos(left_angle) * (gSpellConstants.kEarthDashSpeed + gPlayerSpellConstants[player->index].kEarthDashSpeed);
			player->velocity.y += sin(left_angle) * (gSpellConstants.kEarthDashSpeed + gPlayerSpellConstants[player->index].kEarthDashSpeed);
		}
		else //if it is centered: dash to where you are looking
		{
			player->velocity.x += cos(player->angle) * (gSpellConstants.kEarthDashSpeed + gPlayerSpellConstants[player->index].kEarthDashSpeed);
			player->velocity.y += sin(player->angle) * (gSpellConstants.kEarthDashSpeed + gPlayerSpellConstants[player->index].kEarthDashSpeed);
		}

		cooldown[1] = gSpellConstants.kEarthDashCooldown + gPlayerSpellConstants[player->index].kEarthDashCooldown;
		map->sounds.play(spellSounds::windDash, 0.0f, 50.0f);
	}
}

void WaterElement::projectile(Player * player, Map * map)
{
	
	if (cooldown[0] <= 0.0f || this->active_projectile != nullptr)
	{
		auto position = player->position;
		auto angle = player->angle;
		auto radius = player->radius;
		WaterProjectileSpell *spell = nullptr;
		XMFLOAT2 v;

		if (pUpgrades[player->index].choice[0] == 2)
		{
			if (this->active_projectile == nullptr)
			{
				WaterProjectileSpell *spell = new WaterProjectileSpell(player,
				{
					position.x + cos(angle) * (radius + 0.4f),
					0,
					position.z + sin(angle) * (radius + 0.4f)
				},
				{ cos(angle) * (gSpellConstants.kWaterProjectileSpeed + gPlayerSpellConstants[player->index].kWaterProjectileSpeed),
					sin(angle) * (gSpellConstants.kWaterProjectileSpeed + gPlayerSpellConstants[player->index].kWaterProjectileSpeed) },
					0.1f
				);
				spell->angle = angle;
				map->add_entity(spell);
				active_projectile = spell;
				map->sounds.play(spellSounds::waterProjectile, 0.0f, 50.0f);
				cooldown[0] = gSpellConstants.kWaterProjectileCooldown + gPlayerSpellConstants[player->index].kWaterProjectileCooldown;
			}
			else
			{
				if (this->active_projectile->dead != true)
				{
					position = this->active_projectile->position;
					angle = this->active_projectile->angle;
					radius = this->active_projectile->radius;

					for (size_t i = 0; i < gSpellConstants.kWaterProjectileNrOfShards + gPlayerSpellConstants[player->index].kWaterProjectileNrOfShards + 1; i++)
					{
						v = { cos(angle) * (gSpellConstants.kWaterProjectileSpeed + gPlayerSpellConstants[player->index].kWaterProjectileSpeed), sin(angle) * (gSpellConstants.kWaterProjectileSpeed + gPlayerSpellConstants[player->index].kWaterProjectileSpeed) };

						if (i != 0)
						{
							if (i % 2 != 0)
							{
								v.x = v.x * cos(i * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180) - v.y* sin(i * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180);
								v.y = v.x * sin(i * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180) + v.y * cos(i * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180);
							}
							else
							{
								v.x = v.x * cos((i - 1) * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180) + v.y* sin((i - 1) * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180);
								v.y = v.x * -sin((i - 1) * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180) + v.y * cos((i - 1) * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180);
							}
						}
						spell = new WaterProjectileSpell(player,
						{
							position.x + cos(angle) * (radius + 0.4f) ,
							0,
							position.z + sin(angle) * (radius + 0.4f)
						},
							v,
							0.1f
						);

						map->add_entity(spell);
					}
					this->active_projectile->dead = true; 
					this->active_projectile = nullptr;
				}
				else
				{
					this->active_projectile->dead = true;
					this->active_projectile = nullptr;
				}


				//cooldown[0] = gSpellConstants.kWaterProjectileCooldown + gPlayerSpellConstants[player->index].kWaterProjectileCooldown;
				map->sounds.play(spellSounds::waterProjectile, 0.0f, 45.0f);
			}
			

		}
		else
		{
			for (size_t i = 0; i < gSpellConstants.kWaterProjectileNrOfShards + gPlayerSpellConstants[player->index].kWaterProjectileNrOfShards + 1; i++)
			{
				v = { cos(angle) * (gSpellConstants.kWaterProjectileSpeed + gPlayerSpellConstants[player->index].kWaterProjectileSpeed), sin(angle) * (gSpellConstants.kWaterProjectileSpeed + gPlayerSpellConstants[player->index].kWaterProjectileSpeed) };
				if (i != 0)
				{
					if (i % 2 != 0)
					{
						v.x = v.x * cos(i * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180) - v.y* sin(i * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180);
						v.y = v.x * sin(i * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180) + v.y * cos(i * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180);

					}
					else
					{
						v.x = v.x * cos((i - 1) * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180) + v.y* sin((i - 1) * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180);
						v.y = v.x * -sin((i - 1) * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180) + v.y * cos((i - 1) * (gSpellConstants.kWaterProjectileSpreadAngle + gPlayerSpellConstants[player->index].kWaterProjectileSpreadAngle) * XM_PI / 180);
					}
				}
				spell = new WaterProjectileSpell(player,
				{
					position.x + cos(angle) * (radius + 0.4f) ,
					0,
					position.z + sin(angle) * (radius + 0.4f)
				},
					v,
					0.1f
				);

				map->add_entity(spell);

			}

			cooldown[0] = gSpellConstants.kWaterProjectileCooldown + gPlayerSpellConstants[player->index].kWaterProjectileCooldown;
			map->sounds.play(spellSounds::waterProjectile, 0.0f, 45.0f);
		}
		

		
	}
}

void WaterElement::stomp(Player * player, Map * map)
{
	if (cooldown[2] <= 0.0f)
	{
		player->stomped = true;
		auto nearby = map->get_entities_in_radius(player, gSpellConstants.kEarthStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby)
		{
			result.entity->velocity.x += cos(result.angle) * (gSpellConstants.kWaterStompStrenght + gPlayerSpellConstants[player->index].kWaterStompStrenght) * abs((gSpellConstants.kWaterStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance + gSpellConstants.kWaterStompStrenghtFalloff + gPlayerSpellConstants[player->index].kWaterStompStrenghtFalloff) - result.distance);
			result.entity->velocity.y += sin(result.angle) * (gSpellConstants.kWaterStompStrenght + gPlayerSpellConstants[player->index].kWaterStompStrenght) * abs((gSpellConstants.kWaterStompDistance + gPlayerSpellConstants[player->index].kEarthStompDistance + gSpellConstants.kWaterStompStrenghtFalloff + gPlayerSpellConstants[player->index].kWaterStompStrenghtFalloff) - result.distance);
		}

		cooldown[2] = gSpellConstants.kWaterStompCooldown + gPlayerSpellConstants[player->index].kWaterStompCooldown;
		map->sounds.play(spellSounds::arcaneStomp, 0.0f, 80.0f);
	}
}

void WaterElement::wall(Player * player, Map * map)
{
	if (cooldown[3] <= 0.f) {
		auto position = player->position;
		auto angle = player->angle;
		auto radius = player->radius;

		XMVECTOR pos = {
			position.x + cos(angle) * (radius + 1.5f),
			0,
			position.z + sin(angle) * (radius + 1.5f)
		};

		XMVECTOR dist = pos - XMLoadFloat3(&position);
		XMVECTOR n = XMVector3Cross(dist, { 0, 1, 0 });

		int nrOfPillars = gSpellConstants.kArcaneWallNrOfPillars + gPlayerSpellConstants[player->index].kArcaneWallNrOfPillars;

		for (int i = 0; i < nrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - (gSpellConstants.kFireWallNrOfPillars + gPlayerSpellConstants[player->index].kFireWallNrOfPillars) / 2.f) *
				(gSpellConstants.kFireWallPillarDistance + gPlayerSpellConstants[player->index].kFireWallPillarDistance) / 2.f + pos);

			Entity* e = map->add_entity(new ArcaneWallSpell(player, p,
				gSpellConstants.kArcaneWallPillarRadius + gPlayerSpellConstants[player->index].kArcaneWallPillarRadius));
			if (i == 0 || i == nrOfPillars - 1)
			{
				ArcaneWallSpell* w = dynamic_cast<ArcaneWallSpell*>(e);
				w->edge = true;
			}
		}

		cooldown[3] = gSpellConstants.kWaterWallCooldown + gPlayerSpellConstants[player->index].kWaterWallCooldown;
		map->sounds.play(spellSounds::earthWall, 0.0f, 100.0f);
	}
}

void WaterElement::dash(Player * player, Map * map)
{
	if (cooldown[1] <= 0.f) {
		auto index = player->index;
		XMFLOAT2 leftVector = gGamepads[index]->get_left_thumb();

		//check if left stick is centered
		if (leftVector.x != 0.f && leftVector.y != 0.f)//if it is not: dash to where you are walking
		{
			float left_angle = gGamepads[index]->get_left_thumb_angle();
			player->velocity.x += cos(left_angle) * (gSpellConstants.kWaterDashSpeed + gPlayerSpellConstants[player->index].kWaterDashSpeed);
			player->velocity.y += sin(left_angle) * (gSpellConstants.kWaterDashSpeed + gPlayerSpellConstants[player->index].kWaterDashSpeed);
		}
		else //if it is centered: dash to where you are looking
		{
			player->velocity.x += cos(player->angle) * (gSpellConstants.kWaterDashSpeed + gPlayerSpellConstants[player->index].kWaterDashSpeed);
			player->velocity.y += sin(player->angle) * (gSpellConstants.kWaterDashSpeed + gPlayerSpellConstants[player->index].kWaterDashSpeed);
		}

		cooldown[1] = gSpellConstants.kWaterDashCooldown + gPlayerSpellConstants[player->index].kWaterDashCooldown;
		map->sounds.play(spellSounds::windDash, 0.0f, 50.0f);
	}
}
