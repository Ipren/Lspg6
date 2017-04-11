#include "Game.h"

#include <d3d11.h>
#include <Xinput.h>
#include "dxerr.h"

#include "Globals.h"
#include "Helpers.h"
#include "Camera.h"
#include "Gamepad.h"
#include "Constants.h"
#include "Player.h"

#include "imgui.h"


Gamepad *gGamepads[4];


Game::Game(HWND wndHandle, int width, int height)
{
	// TODO: memory management
	this->currentMap = new Map();
	this->renderer = new Renderer(wndHandle, width, height);
	camera = new Camera({ 0, 15, -5 }, { 0, 0, 0 }, this->renderer->gDevice);
	for (int i = 0; i < 4; ++i) {
		gGamepads[i] = new Gamepad(i);
	}
	this->heigth = height;
	this->width = width;

	XInputEnable(true);
	this->currentState = GameState::MainMenu;
}

Game::Game()
{

}

Game::~Game()
{
	delete this->renderer;
	delete this->camera;
}

void Game::update(float dt)
{
	{
		ImGui::Begin("Constants");
		
		if (ImGui::CollapsingHeader("Spells")) {
			if (ImGui::CollapsingHeader("Arcane")) {
				ImGui::TextDisabled("Projectile");
				ImGui::SliderFloat("seek strength", &gSpellConstants.kArcaneProjectileSeekStrength, 0.0f, 10.0f);
				ImGui::SliderFloat("seek radius", &gSpellConstants.kArcaneProjectileSeekRadius, 0.0f, 10.0f);
				ImGui::SliderFloat("seek falloff", &gSpellConstants.kArcaneProjectileSeekFalloff, 0.0f, 1.0f);
				ImGui::SliderFloat("strength", &gSpellConstants.kArcaneProjectileStrength, 0.0f, 60.0f);
				ImGui::SliderFloat("speed", &gSpellConstants.kArcaneProjectileSpeed, 0.0f, 40.0f);
				ImGui::SliderFloat("cooldown##Arcane", &gSpellConstants.kArcaneProjectileCooldown, 0.0f, 20.0f);

				ImGui::TextDisabled("Stomp");
				ImGui::SliderFloat("distance", &gSpellConstants.kArcaneStompDistance, 0.0f, 10.0f);
				ImGui::SliderFloat("strength##stomp", &gSpellConstants.kArcaneStompStrength, 0.0f, 10.0f);
				ImGui::SliderFloat("strength falloff", &gSpellConstants.kArcaneStompStrengthFalloff, 0.0f, 10.0f);
				ImGui::SliderFloat("cooldown##Stomp", &gSpellConstants.kArcaneStompCooldown, 0.0f, 20.0f);

				ImGui::TextDisabled("Dash");
				ImGui::SliderFloat("speed##dash", &gSpellConstants.kArcaneDashSpeed, 0.0f, 120.f);
				ImGui::SliderFloat("cooldown##Dash", &gSpellConstants.kArcaneDashCooldown, 0.0f, 20.0f);

				ImGui::TextDisabled("Wall");
				ImGui::SliderFloat("cooldown##Wall", &gSpellConstants.kArcaneWallCooldown, 0.0f, 20.0f);
				ImGui::SliderInt("number of pillars", &gSpellConstants.kArcaneWallNrOfPillars, 1, 20);
				ImGui::SliderFloat("distance between pillars", &gSpellConstants.kArcaneWallPillarDistance, 0, 1);
				ImGui::SliderFloat("pillars radius", &gSpellConstants.kArcaneWallPillarRadius, 0, 5);
			}

			if (ImGui::CollapsingHeader("Fire")) {
				ImGui::TextDisabled("Projectile");

				ImGui::SliderFloat("strength##fire", &gSpellConstants.kFireProjectileStrength, 0.0f, 90.0f);
				ImGui::SliderFloat("speed##fire", &gSpellConstants.kFireProjectileSpeed, 0.0f, 30.0f);
				ImGui::SliderFloat("cooldown##Fire", &gSpellConstants.kFireProjectileCooldown, 0.0f, 15.0f);
				ImGui::SliderFloat("explosion radius##Fire", &gSpellConstants.kFireProjectileExplosionRadius, 0.0f, 6.0f);
				ImGui::SliderFloat("explosion falloff##Fire", &gSpellConstants.kFireProjectileExplosionFalloff, 0.0f, 3.0f);

				ImGui::TextDisabled("Stomp");
				ImGui::SliderFloat("distance##f", &gSpellConstants.kFireStompDistance, 0.0f, 10.0f);
				ImGui::SliderFloat("strength##fstomp", &gSpellConstants.kFireStompStrength, 0.0f, 10.0f);
				ImGui::SliderFloat("strength falloff##f", &gSpellConstants.kFireStompStrengthFalloff, 0.0f, 10.0f);
				ImGui::SliderFloat("cooldown##fStomp", &gSpellConstants.kFireStompCooldown, 0.0f, 20.0f);

				ImGui::TextDisabled("Dash");
				ImGui::SliderFloat("speed##fdash", &gSpellConstants.kFireDashSpeed, 0.0f, 120.f);
				ImGui::SliderFloat("cooldown##fDash", &gSpellConstants.kFireDashCooldown, 0.0f, 20.0f);

				ImGui::TextDisabled("Wall");
				ImGui::SliderFloat("cooldown##fWall", &gSpellConstants.kFireWallCooldown, 0.0f, 20.0f);
				ImGui::SliderInt("number of pillars##f", &gSpellConstants.kFireWallNrOfPillars, 1, 20);
				ImGui::SliderFloat("distance between pillars##f", &gSpellConstants.kFireWallPillarDistance, 0, 1);
				ImGui::SliderFloat("pillars radius##f", &gSpellConstants.kFireWallPillarRadius, 0, 5);
			}
		}

		if (ImGui::CollapsingHeader("Player")) {
			ImGui::SliderFloat("radius", &gPlayerConstants.kRadius, 0.0f, 5.f);
			ImGui::SliderFloat("speed##player", &gPlayerConstants.kSpeed, 0.0f, 240);
			ImGui::SliderFloat("friction factor", &gPlayerConstants.kFriction, 0.0f, 30.0f);
		}
		if (ImGui::CollapsingHeader("Game")) {
			ImGui::TextDisabled("Camera");
			
			float p[3] = {
				gGameConstants.kCameraX,
				gGameConstants.kCameraY,
				gGameConstants.kCameraZ
			};
			ImGui::SliderFloat3("pos", p, -30.f, 30.f);
			
			ImGui::SliderFloat("speed##camera", &gGameConstants.kCameraSpeed, 0.f, 5.f);
			ImGui::SliderFloat("drag", &gGameConstants.kCameraDrag, 0.f, 1.f);

			gGameConstants.kCameraX = p[0];
			gGameConstants.kCameraY = p[1];
			gGameConstants.kCameraZ	= p[2];
			
			camera->pos = {
				gGameConstants.kCameraX,
				gGameConstants.kCameraY,
				gGameConstants.kCameraZ
			};

			ImGui::TextDisabled("Gameplay");
			ImGui::Checkbox("can die", &gGameConstants.kCanDie);

			ImGui::TextDisabled("Map");
			ImGui::SliderFloat("Shrink amount", &gMapConstants.kShrinkAmount, 0.0f, 5.0f);
			ImGui::SliderFloat("shrinkl time", &gMapConstants.kShrinkTimer, 0.0f, 60.0f);
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Reset")) {
			if (ImGui::Button("Spells")) {
				gSpellConstants = gDefaultSpellConstants;
			}
			if (ImGui::Button("Players")) {
				gPlayerConstants = gDefaultPlayerConstants;
			}
			if (ImGui::Button("Game##header")) {
				gGameConstants = gDefaultGameConstants;
			}
			if (ImGui::Button("Map"))
			{
				gMapConstants = gDefaultMapConstants;
			}
		}

		ImGui::End();
	}

	{
		ImGui::Begin("Debug");
		if (ImGui::Button("Reset to 2p")) {//resetting the map with 2 players
			currentMap->reset(2);
		}
		if (ImGui::Button("Reset to 3p")) {//resetting the map with 3 players
			currentMap->reset(3);
		}
		if (ImGui::Button("Reset to 4p")) {//resetting the map with 4 players
			currentMap->reset(4);
		}
		ImGui::End();
	}

	if (currentState == GameState::MainMenu)
	{
		ImGui::Begin("Main Menu");
		if (ImGui::Button("Start Game 2p")) {//starting the game with 2 players
			currentState = GameState::Playing;
			currentMap->reset(2);
		}
		if (ImGui::Button("Start Game 3p")) {//starting the game with 3 players
			currentState = GameState::Playing;
			currentMap->reset(3);
		}
		if (ImGui::Button("Start Game 4p")) {//starting the game with 4 players
			currentState = GameState::Playing;
			currentMap->reset(4);
		}
		ImGui::End();
	}else if(currentState == GameState::Playing)
	{ 
		for (int i = 0; i < 4; ++i) {
			gGamepads[i]->update(dt);
		}
		currentMap->update(dt, camera);
		if (currentMap->nrOfAlivePlayers < 2)
		{
			for (int i = 0; i < this->currentMap->entitys.size(); i++)
			{
				if (this->currentMap->entitys[i]->type == EntityType::Player)//adding points to the winning player
				{
					Player* p = dynamic_cast<Player*>(this->currentMap->entitys[i]);
					currentMap->playerPoints[p->index]++;
				
					if (currentMap->playerPoints[p->index] == 3)
					{
						currentState = GameState::EndGame;
					}
					else
					{

					}
				}
			}
			currentState = GameState::MainMenu;
		}
	}else if (currentState == GameState::UpgradeMenu)
	{

	}
	

	camera->update(dt, this->renderer->gDeviceContext);
	renderer->update(dt, this->currentMap);

	
}

void Game::render()
{

	this->renderer->render(this->currentMap, this->camera);
	ImGui::Render();
	this->renderer->present();
}
