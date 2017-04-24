#include "Element.h"

#include "Spell.h"
#include "Player.h"
#include "Constants.h"

void ArcaneElement::projectile(Player *player, Map *map)
{
	if (cooldown[0] <= 0.f) {
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
		cooldown[0] = gSpellConstants.kArcaneProjectileCooldown + gPlayerSpellConstants[player->index].kArcaneProjectileCooldown;
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
	}
}

void ArcaneElement::wall(Player *player, Map *map)
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

		for (int i = 0; i < gSpellConstants.kArcaneWallNrOfPillars + gPlayerSpellConstants[player->index].kArcaneWallNrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - (gSpellConstants.kArcaneWallNrOfPillars + gPlayerSpellConstants[player->index].kArcaneWallNrOfPillars) / 2.f) *
				(gSpellConstants.kArcaneWallPillarDistance + gPlayerSpellConstants[player->index].kArcaneWallPillarDistance)/ 2.f + pos);
			map->add_entity(new ArcaneWallSpell(player, p, gSpellConstants.kArcaneWallPillarRadius + gPlayerSpellConstants[player->index].kArcaneWallPillarRadius));
		}

		cooldown[3] = gSpellConstants.kArcaneWallCooldown + gPlayerSpellConstants[player->index].kArcaneWallCooldown;
	}
}

void ArcaneElement::dash(Player * player, Map * map)
{
	if (cooldown[1] <= 0.f) {
		auto index = player->index;
		XMFLOAT2 leftVector = gGamepads[index]->get_left_thumb();

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

		cooldown[1] = gSpellConstants.kArcaneDashCooldown + gPlayerSpellConstants[player->index].kArcaneDashCooldown;
	}
}

FireElement::FireElement()
	: active_projectile(nullptr)
{
	this->startHealth = 10.f;
}

void FireElement::projectile(Player * player, Map * map)
{
	if (active_projectile == nullptr) {
		if (cooldown[0] <= 0.f) {
			auto position = player->position;
			auto angle = player->angle;
			auto radius = player->radius;

			FireProjectileSpell *spell = new FireProjectileSpell(player,
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
			active_projectile = spell;
		}
	}
	else {
		if (active_projectile->dead != true)
		{
			if (active_projectile->on_effect(map)) {
				cooldown[0] = gSpellConstants.kFireProjectileCooldown + gPlayerSpellConstants[player->index].kFireProjectileCooldown;
				{
					player->blowUp = true;
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
		}

		cooldown[2] = gSpellConstants.kFireStompCooldown + gPlayerSpellConstants[player->index].kFireStompCooldown;
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

		for (int i = 0; i < gSpellConstants.kFireWallNrOfPillars + gPlayerSpellConstants[player->index].kFireWallNrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - (gSpellConstants.kFireWallNrOfPillars + gPlayerSpellConstants[player->index].kFireWallNrOfPillars) / 2.f) *
				(gSpellConstants.kFireWallPillarDistance + gPlayerSpellConstants[player->index].kFireWallPillarDistance) / 2.f + pos);

			// TODO: fire wall
			map->add_entity(new ArcaneWallSpell(player, p, gSpellConstants.kFireWallPillarRadius + gPlayerSpellConstants[player->index].kFireWallPillarRadius));
		}

		cooldown[3] = gSpellConstants.kFireWallCooldown + gPlayerSpellConstants[player->index].kFireWallCooldown;
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
	}
}

//continue from here Andreas
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
		{ cos(angle) * gSpellConstants.kWindProjectileSpeed, sin(angle) * gSpellConstants.kWindProjectileSpeed },
			0.1f
		);

		map->add_entity(spell);
		cooldown[0] = gSpellConstants.kWindProjectileCooldown;
	}
}

void WindElement::stomp(Player * player, Map * map)
{
	if (cooldown[2] <= 0.f) {

		player->stomped = true;
		//saves nearby players in a vector
		auto nearby = map->get_entities_in_radius(player, gSpellConstants.kWindStompDistance, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby) { //moves all nearby players
			result.entity->velocity.x += cos(result.angle) * gSpellConstants.kWindStompStrength * abs((gSpellConstants.kWindStompDistance + gSpellConstants.kWindStompStrengthFalloff) - result.distance);
			result.entity->velocity.y += sin(result.angle) * gSpellConstants.kWindStompStrength * abs((gSpellConstants.kWindStompDistance + gSpellConstants.kWindStompStrengthFalloff) - result.distance);
		}

		cooldown[2] = gSpellConstants.kWindStompCooldown;
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

		for (int i = 0; i < gSpellConstants.kWindWallNrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - gSpellConstants.kWindWallNrOfPillars / 2.f) *
				gSpellConstants.kWindWallPillarDistance / 2.f + pos);

			// TODO: Wind wall
			map->add_entity(new ArcaneWallSpell(player, p, gSpellConstants.kWindWallPillarRadius));
		}

		cooldown[3] = gSpellConstants.kWindWallCooldown;
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
			player->velocity.x += cos(left_angle) * gSpellConstants.kWindDashSpeed;
			player->velocity.y += sin(left_angle) * gSpellConstants.kWindDashSpeed;
		}
		else //if it is centered: dash to where you are looking
		{
			player->velocity.x += cos(player->angle) * gSpellConstants.kWindDashSpeed;
			player->velocity.y += sin(player->angle) * gSpellConstants.kWindDashSpeed;
		}

		cooldown[1] = gSpellConstants.kWindDashCooldown;
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
		{ cos(angle) * gSpellConstants.kEarthProjectileSpeed, sin(angle) * gSpellConstants.kEarthProjectileSpeed },
			0.1f
		);

		map->add_entity(spell);
		cooldown[0] = gSpellConstants.kEarthProjectileCooldown;
	}
}

void EarthElement::stomp(Player * player, Map * map)
{
	if (cooldown[2] <= 0.f) {

		player->stomped = true;
		//saves nearby players in a vector
		auto nearby = map->get_entities_in_radius(player, gSpellConstants.kEarthStompDistance, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby) { //moves all nearby players
			result.entity->velocity.x += cos(result.angle) * gSpellConstants.kEarthStompStrength * abs((gSpellConstants.kEarthStompDistance + gSpellConstants.kEarthStompStrengthFalloff) - result.distance);
			result.entity->velocity.y += sin(result.angle) * gSpellConstants.kEarthStompStrength * abs((gSpellConstants.kEarthStompDistance + gSpellConstants.kEarthStompStrengthFalloff) - result.distance);
		}

		cooldown[2] = gSpellConstants.kEarthStompCooldown;
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

		for (int i = 0; i < gSpellConstants.kEarthWallNrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - gSpellConstants.kEarthWallNrOfPillars / 2.f) *
				gSpellConstants.kEarthWallPillarDistance / 2.f + pos);

			// TODO: Earth wall
			map->add_entity(new ArcaneWallSpell(player, p, gSpellConstants.kEarthWallPillarRadius));
		}

		cooldown[3] = gSpellConstants.kEarthWallCooldown;
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
			player->velocity.x += cos(left_angle) * gSpellConstants.kEarthDashSpeed;
			player->velocity.y += sin(left_angle) * gSpellConstants.kEarthDashSpeed;
		}
		else //if it is centered: dash to where you are looking
		{
			player->velocity.x += cos(player->angle) * gSpellConstants.kEarthDashSpeed;
			player->velocity.y += sin(player->angle) * gSpellConstants.kEarthDashSpeed;
		}

		cooldown[1] = gSpellConstants.kEarthDashCooldown;
	}
}

void WaterElement::projectile(Player * player, Map * map)
{
	if (cooldown[0] <= 0.0f)
	{
		auto position = player->position;
		auto angle = player->angle;
		auto radius = player->radius;
		WaterProjectileSpell *spell = nullptr;
		XMFLOAT2 v;

		for (size_t i = 0; i < gSpellConstants.kWaterProjectileNrOfShards + 1; i++)
		{
			v = { cos(angle) * gSpellConstants.kWaterProjectileSpeed, sin(angle) * gSpellConstants.kWaterProjectileSpeed };
			if (i != 0)
			{
				if (i % 2 != 0)
				{
					v.x = v.x * cos(i * gSpellConstants.kWaterProjectileSpreadAngle * XM_PI / 180) - v.y* sin(i * gSpellConstants.kWaterProjectileSpreadAngle* XM_PI / 180);
					v.y = v.x * sin(i * gSpellConstants.kWaterProjectileSpreadAngle * XM_PI / 180) + v.y * cos(i * gSpellConstants.kWaterProjectileSpreadAngle * XM_PI / 180);

				}
				else
				{
					v.x = v.x * cos((i - 1)* gSpellConstants.kWaterProjectileSpreadAngle * XM_PI / 180) + v.y* sin((i - 1) * gSpellConstants.kWaterProjectileSpreadAngle * XM_PI / 180);
					v.y = v.x * -sin((i - 1) * gSpellConstants.kWaterProjectileSpreadAngle * XM_PI / 180) + v.y * cos((i - 1) * gSpellConstants.kWaterProjectileSpreadAngle * XM_PI / 180);
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
		
		cooldown[0] = gSpellConstants.kWaterProjectileCooldown;
	}
}

void WaterElement::stomp(Player * player, Map * map)
{
	if (cooldown[2] <= 0.0f)
	{
		player->stomped = true;
		auto nearby = map->get_entities_in_radius(player, gSpellConstants.kEarthStompDistance, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby)
		{
			result.entity->velocity.x += cos(result.angle) * gSpellConstants.kWaterStompStrenght * abs((gSpellConstants.kWaterStompDistance + gSpellConstants.kWaterStompStrenghtFalloff) - result.distance);
			result.entity->velocity.y += sin(result.angle) * gSpellConstants.kWaterStompStrenght * abs((gSpellConstants.kWaterStompDistance + gSpellConstants.kWaterStompStrenghtFalloff) - result.distance);
		}

		cooldown[2] = gSpellConstants.kWaterStompCooldown;
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

		for (int i = 0; i < gSpellConstants.kWaterWallNrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - gSpellConstants.kWaterWallNrOfPillars / 2.f) *
				gSpellConstants.kWaterWallPillarDistance / 2.f + pos);

			// TODO: Earth wall
			map->add_entity(new ArcaneWallSpell(player, p, gSpellConstants.kWaterWallPillarRadius));
		}

		cooldown[3] = gSpellConstants.kWaterWallCooldown;
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
			player->velocity.x += cos(left_angle) * gSpellConstants.kWaterDashSpeed;
			player->velocity.y += sin(left_angle) * gSpellConstants.kWaterDashSpeed;
		}
		else //if it is centered: dash to where you are looking
		{
			player->velocity.x += cos(player->angle) * gSpellConstants.kWaterDashSpeed;
			player->velocity.y += sin(player->angle) * gSpellConstants.kWaterDashSpeed;
		}

		cooldown[1] = gSpellConstants.kWaterDashCooldown;
	}
}
