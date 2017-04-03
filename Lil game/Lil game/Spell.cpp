#include "Spell.h"

Spell::Spell(XMFLOAT3 position, XMFLOAT2 velocity, float radius, float life)
	: Entity(EntityType::Spell, position, velocity, radius), life(life)
{
}

Spell::~Spell()
{
}

PushSpell::PushSpell(XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(position, velocity, radius, 4.f), explosion_radius(1.5f), strength(1.f)
{
}

PushSpell::~PushSpell()
{
}

void PushSpell::on_effect(Map *map)
{
	auto nearby = map->get_entities_in_radius(this, explosion_radius);

	for (auto result : nearby) {
		result.entity->add_velocity(
			sin(result.angle) * 5,
			cos(result.angle) * 5
		);
	}
}
