#include "Spell.h"

Spell::Spell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius, float life)
	: Entity(EntityType::Spell, position, velocity, radius), owner(owner), life(life)
{
}

Spell::~Spell()
{
}

ArcaneProjectileSpell::ArcaneProjectileSpell(Player *owner, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: Spell(owner, position, velocity, radius, 4.5f), explosion_radius(1.5f), strength(1.f)
{
}

ArcaneProjectileSpell::~ArcaneProjectileSpell()
{
}

void ArcaneProjectileSpell::update(Map *map, float dt)
{
	EntityQueryResult result;
	if (map->get_nearest_entity(this, 5.f, &result, [this](Entity *e) {
		return e->type == EntityType::Player && e != (Entity*)this->owner;
	})) {
		float magnitude = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
		
		float ang = atan2f(velocity.y, velocity.x);
		float an = atan2(sin(result.angle - ang), cos(result.angle - ang));

		float new_angle = fmod((ang + an *4* dt), (XM_PI*2));
		angle = new_angle;
		
		XMFLOAT2 new_vel = {
			cos(new_angle) * magnitude,
			sin(new_angle) * magnitude
		};

		velocity = new_vel;
	}

	Spell::update(map, dt);
}

bool ArcaneProjectileSpell::on_effect(Map *map)
{
	auto nearby = map->get_entities_in_radius(this, explosion_radius, [](Entity *e) {
		return e->type == EntityType::Player;
	});

	for (auto result : nearby) {
		result.entity->velocity.x += cos(result.angle) * 15 * abs(explosion_radius - result.distance);
		result.entity->velocity.y += sin(result.angle) * 15 * abs(explosion_radius - result.distance);
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

