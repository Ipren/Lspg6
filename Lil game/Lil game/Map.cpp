#include "Map.h"
#include <DirectXMath.h>
#include "Player.h"
#include "Spell.h"

using namespace DirectX;

Map::Map()
{
	for (int i = 0; i < 4; ++i) {
		Entity *e = new Player(i, { (float)i, 0, 0 }, { 0, 0 }, 0.5f);
		entitys.push_back(e);
	}
}

Map::~Map()
{
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

				if (sqrt(dx * dx + dz * dz) < (a->radius + b->radius))
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

std::vector<EntityQueryResult> Map::get_entities_in_radius(Entity *self, float radius)
{
	std::vector<EntityQueryResult> entities;

	for (auto entity : this->entitys) {
		if (entity == self) continue;

		auto pos = self->position;

		float dx = pos.x - entity->position.x;
		float dz = pos.z - entity->position.z;

		float dist = sqrt((dx * dx + dz * dz));
		if (dist < radius) {
			EntityQueryResult result;

			result.entity = entity;
			result.distance = dist;
			result.angle = atan2f(entity->position.z - pos.z, entity->position.x - pos.x);

			entities.push_back(result);
		}
	}

	return entities;
}
