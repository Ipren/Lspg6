#include "Game.h"

#include <d3d11.h>
#include <Xinput.h>
#include "dxerr.h"

#include "Globals.h"
#include "Helpers.h"
#include "Camera.h"
#include "Gamepad.h"

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
}

void Game::update(float dt)
{
	for (int i = 0; i < 4; ++i) {
		gGamepads[i]->update(dt);
	}

	currentMap->update(dt, camera);
	camera->update(dt, this->renderer->gDeviceContext);

	
}

void Game::render()
{
	this->renderer->render(this->currentMap, this->camera);
	/*float clear[] = { 0, 0, 0, 1 };

	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clear);
	gDeviceContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	D3D11_VIEWPORT vp;
	vp.Width = WIDTH;
	vp.Height = HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	gDeviceContext->RSSetViewports(1, &vp);
	gDeviceContext->IASetInputLayout(layout);

	UINT32 size = sizeof(float) * 3;
	UINT32 offset = 0u;
	gDeviceContext->IASetVertexBuffers(0, 1, &quad, &size, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gDeviceContext->VSSetShader(vsh, nullptr, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gCamera->wvp_buffer);
	gDeviceContext->PSSetShader(psh, nullptr, 0);

	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencil);

	gDeviceContext->Draw(6, 0);*/

}
