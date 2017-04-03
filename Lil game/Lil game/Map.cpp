#include "Map.h"
#include <DirectXMath.h>
#include "Player.h"

using namespace DirectX;

Map::Map()
{
	for (int i = 0; i < 4; ++i) {
		Entity *e = new Player(i);
		e->position = { 0, 0, 0 };
		e->radius = 1;
		e->angle = 0.f;
		e->type = EntityType::Player;
		entitys.push_back(e);
	}
}

Map::~Map()
{
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
						entitys[j]->add_velocity((entIx - entitys[j]->position.x)*-10, (entIz - entitys[j]->position.z)*-10);
					}

					if (entitys[i]->type == EntityType::Player && entitys[j]->type == EntityType::Spell)
					{

					}

				}
			}
		}

	}

	// TODO: endast players
	std::vector<XMVECTOR> pos;

	for (int i = 0; i < this->entitys.size(); i++)
	{
		pos.push_back(XMLoadFloat3(&entitys[i]->position));
		entitys[i]->update(dt);
	}

	cam->focus(pos);
}