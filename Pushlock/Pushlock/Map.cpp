#include "Map.h"
#include <DirectXMath.h>
#include "Player.h"
#include "Spell.h"
#include "Constants.h"
#include "Upgrades.h"

using namespace DirectX;

Map::Map(GameState * currentState)
{
	this->meshContainer.Init();
	this->reset(4);
	this->nrOfAlivePlayers = 4;
	this->currentState = currentState;
	for (size_t i = 0; i < 4; i++)
	{
		this->playerElemnts[i] = 0;

	}
	this->indexWinner = -1;
	shrinking = false;
	for (size_t i = 0; i < 4; i++)
	{
		this->upgradeChoice[i] = 0;

	}
	this->radius = 10.0f;
	this->newRadius = radius;
	shrunk = false;
	shrinkAmount = gDefaultMapConstants.kShrinkAmount;
	shrinkTimer = gDefaultMapConstants.kShrinkTimer;
	totalTime = 0.0f;
	timeSinceLastShrunk = 0.0f;
}

Map::~Map()
{
}

void Map::reset(int nrOfPlayers)
{
	entitys.clear();
	for (int i = 0; i < nrOfPlayers; ++i) 
	{
		float offset = 3;
		XMFLOAT3 pPos;
		pPos.y = 0;
		if (i == 0)//set player 1 pos
		{
			pPos.x = -offset;
			pPos.z = offset;
		}else if (i == 1)//set player 2 pos
		{
			pPos.x = offset;
			pPos.z = offset;
		}
		else if (i == 2)//set player 3 pos
		{
			pPos.x = -offset;
			pPos.z = -offset;
		}
		else if (i == 3)//set player 4 pos
		{
			pPos.x = offset;
			pPos.z = -offset;
		}
		Entity *e = new Player(i, pPos, { 0, 0 }, gPlayerConstants.kRadius);
		Player* p = dynamic_cast<Player*>(e);
		if (this->playerElemnts[i] == 0)
		{
			p->element = new ArcaneElement();
		}
		if (this->playerElemnts[i] == 1)
		{
			p->element = new FireElement();
			p->element->startHealth += gPlayerSpellConstants[p->index].kHealth;
		}
		if (this->playerElemnts[i] == 2)
		{
			p->element = new WindElement();
		}
		if (this->playerElemnts[i] == 3)
		{
			p->element = new EarthElement();
		}
		if (this->playerElemnts[i] == 4)
		{
			p->element = new WaterElement();
		}
		p->health = p->element->startHealth;//set health based on the element choice
		entitys.push_back(e);
		//entitys.back()->pMesh = new Mesh();
		//entitys.back()->pMesh->LoadStatic("character.G6Mesh", globalDevice, globalDeviceContext);
		//
		entitys.back()->AssignStaticMesh("character");
		
	}
	this->nrOfAlivePlayers = nrOfPlayers;
	this->nrOfPlayers = nrOfPlayers;
	totalTime = 0.0f;
	timeSinceLastShrunk = 0.0f;
	radius = 10.0f;
	shrunk = true;
	shrinking = false;
	shrinkAmount = gDefaultMapConstants.kShrinkAmount;
	shrinkTimer = gDefaultMapConstants.kShrinkTimer;
}

Entity* Map::add_entity(Entity * entity)
{
	entitys_to_add.push_back(entity);
	return entity;
}

void Map::update(float dt, Camera *cam)
{
	shrinkAmount = gMapConstants.kShrinkAmount;
	shrinkTimer = gMapConstants.kShrinkTimer;
	totalTime += dt;
	timeSinceLastShrunk += dt;
	if (timeSinceLastShrunk > shrinkTimer && shrinking == false)
	{
		timeSinceLastShrunk = 0.0f;
		if (radius - shrinkAmount > 4.0f)
		{
			shrinking = true;
			newRadius = radius - shrinkAmount;
			
		}
	}
	if (shrinking)
	{
		if ( radius - newRadius > 0.001f)
		{
			radius -= 0.001;
			shrunk = true;
			timeSinceLastShrunk = 0.0f;
		}
		else
		{
			shrinking = false;
		}
	}

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

				if (distance < (a->radius + b->radius) && a->type != EntityType::Wall)
				{
					// Player vs. Player
					if (a->type == EntityType::Player && b->type == EntityType::Player)
					{
						//changing the acceleration to negative to create a small bounce effect
						if (dynamic_cast<Player *>(a)->frosen)
						{
							b->acceleration.x = -(a->position.x - b->position.x) * 5328;
							b->acceleration.y = -(a->position.z - b->position.z) * 5328;
						}
						else
						{
							b->acceleration.x = -(a->position.x - b->position.x) * 150;
							b->acceleration.y = -(a->position.z - b->position.z) * 150;
						}
						
					}
					// Player vs. Spell
					else if (a->type == EntityType::Player && b->type == EntityType::Spell)
					{
						Spell *spell = dynamic_cast<Spell*>(b);
						if (dynamic_cast<Player*>(a)->dashing)
						{
							b->acceleration.x = -b->acceleration.x;
							b->acceleration.y = -b->acceleration.y;
						}
						else
						{
							if (spell->on_effect(this)) {//calling spell effect
								spell->dead = true;//deleting the spell
							}
						}
					}

				}//wall checks
				else if (a->type == EntityType::Wall && b->type != EntityType::Wall) 
				{
					ArcaneWallSpell* awall = dynamic_cast<ArcaneWallSpell*>(a);
					EarthWallSpell* ewall = dynamic_cast<EarthWallSpell*>(a);
					FireWallSpell* fwall = dynamic_cast<FireWallSpell*>(a);

				
							if (awall != nullptr)
							{
								awall->on_effect(this);
							}
							else if (ewall != nullptr)
							{
								ewall->on_effect(this);
							}
							else if (fwall != nullptr)
							{
								fwall->on_effect(this);
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
