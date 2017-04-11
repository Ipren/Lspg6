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
		{ cos(angle) * gSpellConstants.kArcaneProjectileSpeed, sin(angle) * gSpellConstants.kArcaneProjectileSpeed },
			0.1f
		);

		map->add_entity(spell);
		cooldown[0] = gSpellConstants.kArcaneProjectileCooldown;
	}
}

void ArcaneElement::stomp(Player *player, Map *map)
{
	if (cooldown[2] <= 0.f) {

		//saves nearby players in a vector
		auto nearby = map->get_entities_in_radius(player, gSpellConstants.kArcaneStompDistance, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby) { //moves all nearby players
			result.entity->velocity.x += cos(result.angle) * gSpellConstants.kArcaneStompStrength * abs((gSpellConstants.kArcaneStompDistance + gSpellConstants.kArcaneStompStrengthFalloff) - result.distance);
			result.entity->velocity.y += sin(result.angle) * gSpellConstants.kArcaneStompStrength * abs((gSpellConstants.kArcaneStompDistance + gSpellConstants.kArcaneStompStrengthFalloff) - result.distance);
		}

		cooldown[2] = gSpellConstants.kArcaneStompCooldown;
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

		for (int i = 0; i < gSpellConstants.kArcaneWallNrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - gSpellConstants.kArcaneWallNrOfPillars / 2.f) *
				gSpellConstants.kArcaneWallPillarDistance / 2.f + pos);
			map->add_entity(new ArcaneWallSpell(player, p, gSpellConstants.kArcaneWallPillarRadius));
		}

		cooldown[3] = gSpellConstants.kArcaneWallCooldown;
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
			player->velocity.x += cos(left_angle) * gSpellConstants.kArcaneDashSpeed;
			player->velocity.y += sin(left_angle) * gSpellConstants.kArcaneDashSpeed;
		}
		else //if it is centered: dash to where you are looking
		{
			player->velocity.x += cos(player->angle) * gSpellConstants.kArcaneDashSpeed;
			player->velocity.y += sin(player->angle) * gSpellConstants.kArcaneDashSpeed;
		}

		cooldown[1] = gSpellConstants.kArcaneDashCooldown;
	}
}

FireElement::FireElement()
	: active_projectile(nullptr)
{
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
			{ cos(angle) * gSpellConstants.kArcaneProjectileSpeed, sin(angle) * gSpellConstants.kArcaneProjectileSpeed },
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
				cooldown[0] = gSpellConstants.kFireProjectileCooldown;
				//if (active_projectile->dead != true)
				{
					player->blowUp = true;
					active_projectile->dead = true;
				}
				/*else
				{
					active_projectile->dead = true;
					active_projectile = nullptr;
				}*/
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

		//saves nearby players in a vector
		auto nearby = map->get_entities_in_radius(player, gSpellConstants.kArcaneStompDistance, [](Entity *e) {
			return e->type == EntityType::Player;
		});

		for (auto result : nearby) { //moves all nearby players
			result.entity->velocity.x += cos(result.angle) * gSpellConstants.kFireStompStrength * abs((gSpellConstants.kFireStompDistance + gSpellConstants.kFireStompStrengthFalloff) - result.distance);
			result.entity->velocity.y += sin(result.angle) * gSpellConstants.kFireStompStrength * abs((gSpellConstants.kFireStompDistance + gSpellConstants.kFireStompStrengthFalloff) - result.distance);
		}

		cooldown[2] = gSpellConstants.kFireStompCooldown;
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

		for (int i = 0; i < gSpellConstants.kFireWallNrOfPillars; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * ((float)i - gSpellConstants.kFireWallNrOfPillars / 2.f) *
				gSpellConstants.kFireWallPillarDistance / 2.f + pos);

			// TODO: fire wall
			map->add_entity(new ArcaneWallSpell(player, p, gSpellConstants.kFireWallPillarRadius));
		}

		cooldown[3] = gSpellConstants.kFireWallCooldown;
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
			player->velocity.x += cos(left_angle) * gSpellConstants.kFireDashSpeed;
			player->velocity.y += sin(left_angle) * gSpellConstants.kFireDashSpeed;
		}
		else //if it is centered: dash to where you are looking
		{
			player->velocity.x += cos(player->angle) * gSpellConstants.kFireDashSpeed;
			player->velocity.y += sin(player->angle) * gSpellConstants.kFireDashSpeed;
		}

		cooldown[1] = gSpellConstants.kFireDashCooldown;
	}
}
