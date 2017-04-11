#pragma once
#include "Map.h"
#include "Renderer.h"
#include "Camera.h"
class Game
{
private:
	enum GameState {
		MainMenu,
		Playing,
		UpgradeMenu
	};
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
	
	GameState currentState;
};

