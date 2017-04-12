#pragma once
#include "Map.h"
#include "Renderer.h"
#include "Camera.h"
class Game
{
private:
	
public:
	Game(HWND wndHandle, int width, int height);
	Game();
	~Game();

	void update(float dt);
	void render();

//private: 
	Map *currentMap;
	Camera *camera;
	Renderer *renderer;
	int width;
	int heigth;
	bool audioDevicePresent;
	
	GameState currentState;
};

