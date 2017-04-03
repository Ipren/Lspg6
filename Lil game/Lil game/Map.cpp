#include "Map.h"


#include "Player.h"

Map::Map()
{
	for (int i = 0; i < 4; ++i) {
		Entity *e = new Player(i);
		e->position = { 0, 0, 0 };
		e->radious = 1;
		e->angle = 0.f;
		entitys.push_back(e);
	}
}

Map::~Map()
{
}
