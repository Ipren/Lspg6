#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include <cstdio>

#include "dxerr.h"
#include "Globals.h"
#include "Helpers.h"
#include "Camera.h"
#include "Map.h"

class Renderer
{
public:
	Renderer();
	Renderer(HWND wndHandle, int width, int height);
	virtual ~Renderer();

	void render(Map *map, Camera *camera);
	ID3D11Device *gDevice;
	ID3D11DeviceContext *gDeviceContext;

private:
	IDXGISwapChain *gSwapChain;
	
	ID3D11RenderTargetView *gBackbufferRTV;
	ID3D11DepthStencilView *gDepthStencil;

	ID3D11Buffer *debug_map_quad;
	ID3D11InputLayout *debug_map_layout;
	ID3D11VertexShader *debug_map_vsh;
	ID3D11PixelShader *debug_map_psh;

	ID3D11Buffer *debug_entity_circle;
	ID3D11InputLayout *debug_entity_layout;
	ID3D11VertexShader *debug_entity_vsh;
	ID3D11PixelShader *debug_entity_psh;

	int width; 
	int height;

	void create_debug_entity();
	void createShaders();
	void createDepthBuffers();
	HRESULT createDirect3DContext(HWND wndHandle);
	void setViewPort(int width, int height);
	
};
