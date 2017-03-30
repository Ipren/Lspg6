#include "Game.h"

#include <d3d11.h>
#include <Xinput.h>
#include "dxerr.h"

#include "Globals.h"
#include "Helpers.h"
#include "Camera.h"

ID3D11Buffer *quad;
ID3D11InputLayout *layout;
ID3D11VertexShader *vsh;
ID3D11PixelShader *psh;
Camera *gCamera;

Game::Game()
{
	// TODO: @@
	gCamera = new Camera({ 0, 5, -2 }, { 0, 0, 0 });

	float vertices[] = {
		-1, 0, -1,
		-1, 0,  1,
		1, 0,  1,
		 
		-1, 0, -1,
		1, 0,  1,
		1, 0, -1
	};

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(vertices);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &vertices;

	DXCALL(gDevice->CreateBuffer(&desc, &data, &quad));

	ID3DBlob *blob = compile_shader(L"Simple.hlsl", "VS", "vs_5_0");
	DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vsh));
	
	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob);

	blob = compile_shader(L"Simple.hlsl", "PS", "ps_5_0");
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &psh));

	XInputEnable(true);
}

Game::~Game()
{
}

void Game::update(float dt)
{
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	auto result = XInputGetState(0, &state);

	if (result == ERROR_SUCCESS) {
		float LX = state.Gamepad.sThumbLX;
		float LY = state.Gamepad.sThumbLY;

		//determine how far the controller is pushed
		float magnitude = sqrt(LX*LX + LY*LY);

		//determine the direction the controller is pushed
		float normalizedLX = LX / magnitude;
		float normalizedLY = LY / magnitude;
		float normalizedMagnitude = 0;

		//check if the controller is outside a circular dead zone
		if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			//clip the magnitude at its expected maximum value
			if (magnitude > 32767) magnitude = 32767;

			//adjust magnitude relative to the end of the dead zone
			magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

			//optionally normalize the magnitude with respect to its expected range
			//giving a magnitude value of 0.0 to 1.0
			normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		}
		else //if the controller is in the deadzone zero out the magnitude
		{
			magnitude = 0.0;
			normalizedMagnitude = 0.0;
		}

		gCamera->pos += { normalizedLX * normalizedMagnitude / 20.f, 0, normalizedLY * normalizedMagnitude / 20.f, 0 };
		gCamera->look += { normalizedLX * normalizedMagnitude / 20.f, 0, normalizedLY * normalizedMagnitude / 20.f, 0 };
	}

	gCamera->update(dt);
}

void Game::render()
{
	float clear[] = { 0, 0, 0, 1 };

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

	auto size = sizeof(float) * 3;
	auto offset = 0u;
	gDeviceContext->IASetVertexBuffers(0, 1, &quad, &size, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gDeviceContext->VSSetShader(vsh, nullptr, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gCamera->wvp_buffer);
	gDeviceContext->PSSetShader(psh, nullptr, 0);

	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencil);

	gDeviceContext->Draw(6, 0);
}
