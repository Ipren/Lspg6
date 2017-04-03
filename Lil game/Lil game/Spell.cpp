#include "Spell.h"

Spell::Spell(XMFLOAT3 position, XMFLOAT2 velocity, float radius, float life)
	: Entity(EntityType::Spell, position, velocity, radius), life(life)
{
}

Spell::~Spell()
{
}

PushSpell::PushSpell(XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(position, velocity, radius, 2.f), explosion_radius(1.5f), strength(1.f)
{
}

PushSpell::~PushSpell()
{
}

void PushSpell::on_effect(Map *map)
{
	auto nearby = map->get_entities_in_radius(this, explosion_radius);

	for (auto result : nearby) {
		result.entity->velocity.x += cos(result.angle) * 25 * abs(explosion_radius - result.distance);
		result.entity->velocity.y += sin(result.angle) * 25 * abs(explosion_radius - result.distance);
	}
}
