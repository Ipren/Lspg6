#include "Map.h"


#include "Player.h"

Map::Map()
{
	Entity *e = new Player(0);
	e->position = { 0, 0, 0 };
	e->radious = 1;
	e->angle = 0.f;
	entitys.push_back(e);
}

Map::~Map()
{
}
