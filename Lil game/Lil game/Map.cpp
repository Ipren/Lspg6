#include "Map.h"
#include <DirectXMath.h>
#include "Player.h"
#include "Spell.h"
#include "Constants.h"


using namespace DirectX;

Map::Map()
{
	for (int i = 0; i < 4; ++i) {
		Entity *e = new Player(i, { (float)i, 0, 0 }, { 0, 0 }, gPlayerConstants.kRadius);
		e->AddMesh("test.obj");
		entitys.push_back(e);
	}
	
}

Map::~Map()
{
}

void Map::reset()
{
	entitys.clear();
	for (int i = 0; i < 4; ++i) {
		Entity *e = new Player(i, { (float)i, 0, 0 }, { 0, 0 }, gPlayerConstants.kRadius);
		entitys.push_back(e);
	}
}

void Map::add_entity(Entity * entity)
{
	entitys_to_add.push_back(entity);
}

void Map::update(float dt, Camera *cam)
{


	for (int i = 0; i < this->entitys.size(); i++)
	{
		Entity *a = entitys[i];
		float entIx = a->position.x;
		float entIz = a->position.z;


		for (int j = 0; j < entitys.size(); j++)
		{
			if (i != j)
			{
				Entity *b = entitys[j];

				float dx = abs(a->position.x - b->position.x);
				float dz = abs(a->position.z - b->position.z);

				float distance = sqrt(dx * dx + dz * dz);

				if (distance < (a->radius + b->radius))
				{
					// Player vs. Player
					if (a->type == EntityType::Player && b->type == EntityType::Player)
					{
						b->acceleration.x = -(a->position.x - b->position.x) * 150;
						b->acceleration.y = -(a->position.z - b->position.z) * 150;
					}
					// Player vs. Spell
					else if (a->type == EntityType::Player && b->type == EntityType::Spell)
					{
						Spell *spell = dynamic_cast<Spell*>(b);
						if (spell->on_effect(this)) {
							spell->dead = true;
						}
					}
					// Player and Spell vs. Wall
					else if (a->type == EntityType::Wall && (b->type == EntityType::Spell || b->type == EntityType::Player))
					{

						b->acceleration.x = -(a->position.x - b->position.x) * 150;
						b->acceleration.y = -(a->position.z - b->position.z) * 150;

						if (b->acceleration.x > 4 || b->acceleration.y > 4)
						{
							b->velocity.x = -b->velocity.x;
							b->velocity.y = -b->velocity.y;
						}

					}

				}
			}
		}

	}

	// TODO: endast players
	std::vector<XMVECTOR> pos;

	int i = 0;
	auto it = entitys.begin();
	while (it != entitys.end())
	{
		if ((*it)->type == EntityType::Player)
		pos.push_back(XMLoadFloat3(&(*it)->position));
		
		(*it)->update(this, dt);

		if ((*it)->dead) {
			it = entitys.erase(it);
		}
		else {
			it++;
		}
	}

	it = entitys_to_add.begin();
	while (it != entitys_to_add.end())
	{
		entitys.push_back(*it);
		it = entitys_to_add.erase(it);
	}

	cam->focus(pos);
}

std::vector<EntityQueryResult> Map::get_entities_in_radius(Entity *self, float radius, std::function<bool(Entity*)> predicate)
{
	std::vector<EntityQueryResult> entities;

	for (auto entity : this->entitys) {
		if (entity == self) continue;
		if (!predicate(entity)) continue;

		auto pos = self->position;

		float dx = pos.x - entity->position.x;
		float dz = pos.z - entity->position.z;

		float dist = sqrt((dx * dx + dz * dz));
		if (dist < radius + entity->radius) {
			EntityQueryResult result;

			result.entity = entity;
			result.distance = dist;
			result.angle = atan2f(entity->position.z - pos.z, entity->position.x - pos.x);

			entities.push_back(result);
		}
	}

	return entities;
}

bool Map::get_nearest_entity(Entity * self, float radius, EntityQueryResult * result, std::function<bool(Entity*)> predicate)
{
	auto results = get_entities_in_radius(self, radius, predicate);

	if (results.size() > 0) {
		auto it = std::min_element(results.begin(), results.end(), [](EntityQueryResult a, EntityQueryResult b) {
			return a.distance < b.distance;
		});

		*result = *it;
		return true;
	}
	else {
		return false;
	}

}
