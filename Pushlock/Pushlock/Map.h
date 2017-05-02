#pragma once
#include "Camera.h"
#include "Entity.h"
#include <vector>
#include <algorithm>
#include <functional>
#include "SoundSystem.h"

struct EntityQueryResult {
	Entity *entity;
	float angle;
	float distance;
};

class Map
{
public:
	Map(GameState * currentState);
	virtual ~Map();

	void reset(int nrOfPlayers = 4);
	Entity* add_entity(Entity *entity);
	void update(float dt, Camera *cam);

	std::vector<EntityQueryResult> get_entities_in_radius(Entity *self, float radius, std::function<bool(Entity*)> predicate);
	bool get_nearest_entity(Entity *self, float radius, EntityQueryResult *result, std::function<bool (Entity*)> predicate);
//private:
	std::vector <Entity*> entitys;
	std::vector <Entity*> entitys_to_add;
	int nrOfAlivePlayers;
	int playerPoints[4];
	int playerElemnts[4];
	float totalTime;
	float timeSinceLastShrunk;
	float radius;
	bool shrunk;
	bool shrinking;
	float shrinkAmount;
	float shrinkTimer;
	float newRadius;
	GameState* currentState;
	int indexWinner;
	int upgradeChoice[4];

	SoundSystem sounds;
	/*used for starting each round with the same nr of players*/
	int nrOfPlayers;
};
