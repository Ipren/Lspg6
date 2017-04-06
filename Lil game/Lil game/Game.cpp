#include "Game.h"

#include <d3d11.h>
#include <Xinput.h>
#include "dxerr.h"

#include "Globals.h"
#include "Helpers.h"
#include "Camera.h"
#include "Gamepad.h"
#include "Constants.h"

#include "imgui.h"

ID3D11Buffer *quad;
ID3D11InputLayout *layout;
ID3D11VertexShader *vsh;
ID3D11PixelShader *psh;

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
		
		if (ImGui::CollapsingHeader("Arcane")) {
			ImGui::TextDisabled("Projectile");
			ImGui::SliderFloat("seek strength", &gConstants.kArcaneProjectileSeekStrength, 0.0f, 10.0f);
			ImGui::SliderFloat("seek radius", &gConstants.kArcaneProjectileSeekRadius, 0.0f, 10.0f);
			ImGui::SliderFloat("strength", &gConstants.kArcaneProjectileStrength, 0.0f, 30.0f);
			ImGui::SliderFloat("speed", &gConstants.kArcaneProjectileSpeed, 0.0f, 20.0f);

			ImGui::TextDisabled("Stomp");
			ImGui::SliderFloat("distance", &gConstants.kArcaneStompDistance, 0.0f, 10.0f);
			ImGui::SliderFloat("strength##stomp", &gConstants.kArcaneStompStrength, 0.0f, 10.0f);
			ImGui::SliderFloat("strength falloff", &gConstants.kArcaneStompStrengthFalloff, 0.0f, 10.0f);
		}

		ImGui::Separator();

		if (ImGui::Button("Reset")) {
			gConstants = gDefaultConstants;
		}

		ImGui::End();
	}

	{
		ImGui::Begin("Debug");
		if (ImGui::Button("Reset map")) {
			currentMap->reset();
		}
		ImGui::End();
	}

	for (int i = 0; i < 4; ++i) {
		gGamepads[i]->update(dt);
	}

	currentMap->update(dt, camera);
	camera->update(dt, this->renderer->gDeviceContext);
	renderer->updateParticles(dt);

	
}

void Game::render()
{

	this->renderer->render(this->currentMap, this->camera);
	ImGui::Render();
	this->renderer->present();
}
