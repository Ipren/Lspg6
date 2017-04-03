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
		float entIx = entitys[i]->position.x;
		float entIz = entitys[i]->position.z;


		for (int j = 0; j < entitys.size(); j++)
		{
			if (i != j)
			{
				float distanceX = abs(entIx - entitys[j]->position.x);
				float distanceZ = abs(entIz - entitys[j]->position.z);

				if (sqrt(distanceX*distanceX + distanceZ*distanceZ) < entitys[i]->radius + entitys[j]->radius)
				{
					if (entitys[i]->type == EntityType::Player && entitys[j]->type == EntityType::Player)
					{
						entitys[j]->add_velocity((entIx - entitys[j]->position.x)/-10, (entIz - entitys[j]->position.z)/-10);
					}

					if (entitys[i]->type == EntityType::Player && entitys[j]->type == EntityType::Spell)
					{
						Spell *spell = dynamic_cast<Spell*>(entitys[j]);
						spell->on_effect(this);
						spell->dead = true;
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
		if (i++ < 4)
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

		float dx = abs(entity->position.x - pos.x);
		float dz = abs(entity->position.z - pos.z);

		float dist = sqrt((dx * dx + dz * dz));
		if (dist < radius + radius) {
			EntityQueryResult result;

			result.entity = entity;
			result.distance = dist;
			result.angle = atan2f(entity->position.z - pos.z, entity->position.x - pos.x);

			entities.push_back(result);
		}
	}

	return entities;
}
