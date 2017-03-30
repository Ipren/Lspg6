#pragma once
#include "Map.h"
class Game
{
public:
	Game();
	~Game();

	void update(float dt);
	void render();

private: 
	Map* currentMap;
};

