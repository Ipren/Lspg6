#pragma once
#include "Camera.h"
#include "Entity.h"
#include <vector>
#include <algorithm>
#include <functional>

struct EntityQueryResult {
	Entity *entity;
	float angle;
	float distance;
};

class Map
{
public:
	Map(GameState * surrentState);
	virtual ~Map();

	void reset(int nrOfPlayers = 4);
	void add_entity(Entity *entity);
	void update(float dt, Camera *cam);

	std::vector<EntityQueryResult> get_entities_in_radius(Entity *self, float radius, std::function<bool(Entity*)> predicate);
	bool get_nearest_entity(Entity *self, float radius, EntityQueryResult *result, std::function<bool (Entity*)> predicate);
//private:
	std::vector <Entity*> entitys;
	std::vector <Entity*> entitys_to_add;
	int nrOfAlivePlayers;
	int playerPoints[4];
	int round;
	float totalTime;
	float timeSinceLastShrunk;
	float radius;
	bool shrunk;
	float shrinkAmount;
	float shrinkTimer;
	GameState* currentState;
};
