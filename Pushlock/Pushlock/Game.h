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

	bool update(float dt);
	void updateUpgradeStats();
	void render();

//private: 

	Menu* menu;
	int currentMenu;
	Map *currentMap;
	Camera *camera;
	Renderer *renderer;
	int width;
	int heigth;
	int playerElements;
	bool mainMenu_FirstFrame;
	bool playing_firstFrame;
	
	GameState currentState;
};

