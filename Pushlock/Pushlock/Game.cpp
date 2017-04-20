#include "Game.h"

#include <d3d11.h>
#include <Xinput.h>
#include "dxerr.h"

#include "Globals.h"
#include "Helpers.h"
#include "Camera.h"
#include "Gamepad.h"
#include "Constants.h"
#include "Upgrades.h"
#include "Player.h"
#include "Menu.h"
#include <Audio.h>

#include "imgui.h"


Gamepad *gGamepads[4];

bool firsttime = true;
bool test = true;
std::unique_ptr<AudioEngine> audEngine;

Game::Game(HWND wndHandle, int width, int height)
{
	// TODO: memory management
	this->currentState = GameState::MainMenu;
	this->currentMap = new Map(&currentState);
	this->renderer = new Renderer(wndHandle, width, height);
	this->mainMenu = new Menu(renderer);
	this->choosePowers = new Menu(renderer);
	this->choosePowers->menuType = 1;
	this->currentMenu = this->mainMenu;
	camera = new Camera({ 0, 15, -5 }, { 0, 0, 0 }, this->renderer->gDevice);
	for (int i = 0; i < 4; ++i) {
		gGamepads[i] = new Gamepad(i);
	}
	this->heigth = height;
	this->width = width;
	int currentRound = 0;

	XInputEnable(true);
	

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	


		AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
		eflags = eflags | AudioEngine_Debug;
#endif // _DEBUG

		audEngine = std::make_unique<AudioEngine>(eflags);

	
		

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

			if (ImGui::CollapsingHeader("Wind")) 
			{
				ImGui::TextDisabled("Projectile");

				ImGui::SliderFloat("strength##wind", &gSpellConstants.kWindProjectileStrength, 0.0f, 90.0f);
				ImGui::SliderFloat("speed##wind", &gSpellConstants.kWindProjectileSpeed, 0.0f, 30.0f);
				ImGui::SliderFloat("cooldown##wind", &gSpellConstants.kWindProjectileCooldown, 0.0f, 15.0f);

				ImGui::TextDisabled("Stomp");
				ImGui::SliderFloat("distance##w", &gSpellConstants.kWindStompDistance, 0.0f, 10.0f);
				ImGui::SliderFloat("strength##wstomp", &gSpellConstants.kWindStompStrength, 0.0f, 10.0f);
				ImGui::SliderFloat("strength falloff##wstomp", &gSpellConstants.kWindStompStrengthFalloff, 0.0f, 10.0f);
				ImGui::SliderFloat("cooldown##wstomp", &gSpellConstants.kWindStompCooldown, 0.0f, 20.0f);

				ImGui::TextDisabled("Dash");
				ImGui::SliderFloat("speed##wdash", &gSpellConstants.kWindDashSpeed, 0.0f, 120.f);
				ImGui::SliderFloat("cooldown##wdash", &gSpellConstants.kWindDashCooldown, 0.0f, 20.0f);

				ImGui::TextDisabled("Wall");
				ImGui::SliderFloat("cooldown##wwall", &gSpellConstants.kFireWallCooldown, 0.0f, 20.0f);
				ImGui::SliderInt("number of pillars##wwall", &gSpellConstants.kFireWallNrOfPillars, 1, 20);
				ImGui::SliderFloat("distance between pillars##wwall", &gSpellConstants.kFireWallPillarDistance, 0, 1);
				ImGui::SliderFloat("pillars radius##wwall", &gSpellConstants.kFireWallPillarRadius, 0, 5);
			}

			if (ImGui::CollapsingHeader("Earth")) {
				ImGui::TextDisabled("Projectile");

				ImGui::SliderFloat("strength##earth", &gSpellConstants.kEarthProjectileStrength, 0.0f, 90.0f);
				ImGui::SliderFloat("speed##earth", &gSpellConstants.kEarthProjectileSpeed, 0.0f, 30.0f);
				ImGui::SliderFloat("cooldown##earth", &gSpellConstants.kEarthProjectileCooldown, 0.0f, 15.0f);
				ImGui::SliderFloat("effect radius##earth", &gSpellConstants.kEarthProjectileEffectRadius, 0.0f, 6.0f);
				ImGui::SliderFloat("effect falloff##earth", &gSpellConstants.kEarthProjectileEffectFalloff, 0.0f, 3.0f);
				ImGui::SliderFloat("effect arming time##earth", &gSpellConstants.kEarthProjectileEffectArmingTime, 0.0f, 1.0f);


				ImGui::TextDisabled("Stomp");
				ImGui::SliderFloat("distance##e", &gSpellConstants.kEarthStompDistance, 0.0f, 10.0f);
				ImGui::SliderFloat("strength##estomp", &gSpellConstants.kEarthStompStrength, 0.0f, 10.0f);
				ImGui::SliderFloat("strength falloff##estomp", &gSpellConstants.kEarthStompStrengthFalloff, 0.0f, 10.0f);
				ImGui::SliderFloat("cooldown##estomp", &gSpellConstants.kEarthStompCooldown, 0.0f, 20.0f);

				ImGui::TextDisabled("Dash");
				ImGui::SliderFloat("speed##edash", &gSpellConstants.kEarthDashSpeed, 0.0f, 120.f);
				ImGui::SliderFloat("cooldown##eDash", &gSpellConstants.kEarthDashCooldown, 0.0f, 20.0f);

				ImGui::TextDisabled("Wall");
				ImGui::SliderFloat("cooldown##ewall", &gSpellConstants.kEarthWallCooldown, 0.0f, 20.0f);
				ImGui::SliderInt("number of pillars##ewall", &gSpellConstants.kEarthWallNrOfPillars, 1, 20);
				ImGui::SliderFloat("distance between pillars##ewall", &gSpellConstants.kEarthWallPillarDistance, 0, 1);
				ImGui::SliderFloat("pillars radius##ewall", &gSpellConstants.kEarthWallPillarRadius, 0, 5);
			}

			if (ImGui::CollapsingHeader("Water")) {
				ImGui::TextDisabled("Projectile");

				ImGui::SliderFloat("strength##earth", &gSpellConstants.kWaterProjectileStrenght, 0.0f, 90.0f);
				ImGui::SliderFloat("speed##earth", &gSpellConstants.kWaterProjectileSpeed, 0.0f, 30.0f);
				ImGui::SliderFloat("cooldown##earth", &gSpellConstants.kWaterProjectileCooldown, 0.0f, 15.0f);
				ImGui::SliderInt("nr of shards", &gSpellConstants.kWaterProjectileNrOfShards, 1, 10);
				ImGui::SliderFloat("spread Angle", &gSpellConstants.kWaterProjectileSpreadAngle, 1.0f, 15.0f);


				ImGui::TextDisabled("Stomp");
				ImGui::SliderFloat("distance##e", &gSpellConstants.kWaterStompDistance, 0.0f, 10.0f);
				ImGui::SliderFloat("strength##estomp", &gSpellConstants.kWaterStompStrenght, 0.0f, 10.0f);
				ImGui::SliderFloat("strength falloff##estomp", &gSpellConstants.kWaterStompStrenghtFalloff, 0.0f, 10.0f);
				ImGui::SliderFloat("cooldown##estomp", &gSpellConstants.kWaterStompCooldown, 0.0f, 20.0f);

				ImGui::TextDisabled("Dash");
				ImGui::SliderFloat("speed##edash", &gSpellConstants.kWaterDashSpeed, 0.0f, 120.f);
				ImGui::SliderFloat("cooldown##eDash", &gSpellConstants.kWaterDashCooldonw, 0.0f, 20.0f);

				ImGui::TextDisabled("Wall");
				ImGui::SliderFloat("cooldown##ewall", &gSpellConstants.kWaterWallCooldown, 0.0f, 20.0f);
				ImGui::SliderInt("number of pillars##ewall", &gSpellConstants.kWaterWallNrOfPillars, 1, 20);
				ImGui::SliderFloat("distance between pillars##ewall", &gSpellConstants.kWaterWallPillarDistance, 0, 1);
				ImGui::SliderFloat("pillars radius##ewall", &gSpellConstants.kWaterWallPillarRadius, 0, 5);
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

	//if (this->currentMenu != nullptr)
	//	this->currentMenu->update();

	{
		if (ImGui::Button("Main Menu")) {//start the main menu
			currentState = GameState::MainMenu;
		}
		int i = 0;
		for (i; i < currentMap->nrOfAlivePlayers; i++)//showing players health
		{
			Player* p = dynamic_cast<Player*>(currentMap->entitys[i]);
			if(p != nullptr)
				ImGui::Text("player %i: %f", i+1, p->health);
				
		}for (i; i < currentMap->nrOfPlayers; i++)
		{
			ImGui::Text("player %i: dead", i + 1);
		}

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
		currentMenu = mainMenu;
		ImGui::Begin("Main Menu");
		if (ImGui::Button("Start Game 2p")) {//starting the game with 2 players
			currentState = GameState::ChoosePowers;
			currentMap->reset(2);
		}
		if (ImGui::Button("Start Game 3p")) {//starting the game with 3 players
			currentState = GameState::ChoosePowers;
			currentMap->reset(3);
		}
		if (ImGui::Button("Start Game 4p")) {//starting the game with 4 players
			currentState = GameState::ChoosePowers;
			currentMap->reset(4);
			
		}
		ImGui::End();
	}
	else if (currentState == GameState::ChoosePowers)
	{
		currentMenu = choosePowers;
		for (int i = 0; i < 4; ++i) {
			gGamepads[i]->update(dt);
		}
		currentMap->update(dt, camera);
		ImGui::Begin("Choose elemnts");
		ImGui::Text("x - wind");
		ImGui::Text("y - earth");
		ImGui::Text("a - arcane");
		ImGui::Text("b - fire");
		ImGui::Text("rb - water");

		for (int i = 0; i < currentMap->nrOfAlivePlayers; i++)
		{
			Player * p = dynamic_cast<Player*>(currentMap->entitys[i]);
				ImGui::Text("Player %d ready: %d", i, p->ready);
		}

		int readyCount = 0;
		for (int i = 0; i < currentMap->nrOfAlivePlayers; i++)
		{
			if (dynamic_cast<Player*>(currentMap->entitys[i])->ready)
			{
				readyCount++;
			}	
		}
		if (readyCount == currentMap->nrOfAlivePlayers)
		{
				currentState = GameState::Playing;
			currentMap->reset(currentMap->nrOfAlivePlayers);
		}

		if (ImGui::Button("start anyway"))
		{
			currentState = GameState::Playing;
			currentMap->reset(currentMap->nrOfAlivePlayers);
		}
		ImGui::End();
	}
	else if (currentState == GameState::Playing)
	{
		currentMenu = playing;
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
						currentMap->indexWinner = p->index;
					}
					else
					{
						currentState = GameState::EndRound;
					}
				}
			}
		}
	}else if (currentState == GameState::EndRound)//här ska upgraderingar hända
	{
		//make upgrade choices here
		currentMenu = endRound;

		for (int i = 0; i < 4; ++i) {
			gGamepads[i]->update(dt);
		}
		if (firsttime == true)
		{
			currentMap->reset(currentMap->nrOfPlayers);
			firsttime = false;
		}
		currentMap->update(dt, camera);
		ImGui::Begin("End of the round");
		for (int i = 0; i < currentMap->nrOfPlayers; i++)
		{
			ImGui::Text("player %i: %i", i+1, currentMap->playerPoints[i]);
		}
		ImGui::Text("Choose your upgrade");
		ImGui::Text("x - Upgrade 1");
		ImGui::Text("y - Upgrade 2");
		ImGui::Text("a - Upgrade 3");
		ImGui::Text("b - Upgrade 4");

		for (int i = 0; i < currentMap->nrOfPlayers; i++)
		{
			ImGui::Text("player %i choose upgrade %i", i + 1, pUpgrades[i].choice[currentRound]);
		}

		for (int i = 0; i < currentMap->nrOfPlayers; i++)
		{
			Player * p = dynamic_cast<Player*>(currentMap->entitys[i]);
			ImGui::Text("Player %d ready: %d", i, p->ready);
		}

		int readyCount = 0;
		for (int i = 0; i < currentMap->nrOfPlayers; i++)
		{
			if (dynamic_cast<Player*>(currentMap->entitys[i])->ready)
			{
				readyCount++;
			}
		}

		if (readyCount == currentMap->nrOfPlayers)
		{
			for (int i = 0; i < currentMap->nrOfPlayers; i++)
			{
				pUpgrades[i].round++;
			}
			currentRound++;
			firsttime = true;
			currentState = GameState::Playing;
			currentMap->reset(currentMap->nrOfPlayers);
		}

		if (ImGui::Button("start next round")) {

			for (int i = 0; i < currentMap->nrOfPlayers; i++)
			{
				pUpgrades[i].round++;
			}
			firsttime = true;
			currentState = GameState::Playing;
			currentMap->reset(currentMap->nrOfPlayers);
		}
		ImGui::End();
	}
	if (currentState == GameState::EndGame)
	{
		currentMenu = endGame;
		ImGui::Begin("End of the game");
		ImGui::Text("Winner player: %i", currentMap->indexWinner +1);
		if (ImGui::Button("Go to main menu")) {
			currentState = GameState::MainMenu;}
		ImGui::End();
		for (int i = 0; i < 4; i++)
		{
			currentMap->playerPoints[i] = 0;
		}
	}

	std::unique_ptr<SoundEffect> soundEffect;
	std::unique_ptr<SoundEffectInstance> effect;
	if (test)
	{
		soundEffect = std::make_unique<SoundEffect>(audEngine.get(), L"boom.wav");
		effect = soundEffect->CreateInstance();
		test = false;
		//soundEffect->Play();
		effect->Play(true);
	}
	
	if (audEngine->IsAudioDevicePresent())
	{
		if (!audEngine->Update())
		{
			if (audEngine->IsCriticalError())
			{
				MessageBox(0, L"no audio device active", L"error", MB_OK);
			}

		}
	}

	
	
	
	
	camera->update(dt, this->renderer->gDeviceContext);
	renderer->update(dt, this->currentMap);

	
}

void Game::render()
{


	this->renderer->render(this->currentMap, this->camera);
	if (this->currentMenu != nullptr)
		this->currentMenu->render(this->renderer);

	
	ImGui::Render();
	this->renderer->present();
}
