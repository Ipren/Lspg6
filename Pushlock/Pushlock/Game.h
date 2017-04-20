#pragma once
#include "Map.h"
#include "Renderer.h"
#include "Camera.h"
#include "Menu.h"
class Game
{
public:
	Game(HWND wndHandle, int width, int height);
	Game();
	~Game();

	void update(float dt);
	void render();

//private: 
	Menu* endGame;
	Menu* endRound;
	Menu* playing;
	Menu* choosePowers;
	Menu* mainMenu;
	Menu* currentMenu;
	Map *currentMap;
	Camera *camera;
	Renderer *renderer;
	int width;
	int heigth;
	bool audioDevicePresent;
	int playerElements;
	int currentRound;
	
	GameState currentState;
};

