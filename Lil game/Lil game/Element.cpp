#include "Element.h"

#include "Spell.h"
#include "Player.h"
#include "Constants.h"

void ArcaneElement::projectile(Player *player, Map *map)
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
	{ cos(angle) * gConstants.kArcaneProjectileSpeed, sin(angle) * gConstants.kArcaneProjectileSpeed },
		0.1
	);

	map->add_entity(spell);
}

void ArcaneElement::stomp(Player *player, Map *map)
{
	auto nearby = map->get_entities_in_radius(player, gConstants.kArcaneStompDistance, [](Entity *e) {
		return e->type == EntityType::Player;
	});

	for (auto result : nearby) {
		result.entity->velocity.x += cos(result.angle) * gConstants.kArcaneStompStrength * abs((gConstants.kArcaneStompDistance + gConstants.kArcaneStompStrengthFalloff) - result.distance);
		result.entity->velocity.y += sin(result.angle) * gConstants.kArcaneStompStrength * abs((gConstants.kArcaneStompDistance + gConstants.kArcaneStompStrengthFalloff) - result.distance);
	}
}

void ArcaneElement::wall(Player *player, Map *map)
{
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

	for (int i = 0; i < 6; i++)
	{

		XMFLOAT3 p;
		XMStoreFloat3(&p, n * (i - 3)*0.35 + pos);
		map->add_entity(new ArcaneWallSpell(player, p, 0.35));
	}
}
