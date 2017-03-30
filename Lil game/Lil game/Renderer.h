#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include "dxerr.h"
#include <DirectXMath.h>
#include <cstdio>
#include <iostream>
#include "Globals.h"
#include "Helpers.h"
#include "Camera.h"

class Renderer
{
public:
	Renderer();
	Renderer(HWND wndHandle, int width, int height);
	virtual ~Renderer();

	void render(Camera *gCamera);
	ID3D11Device *gDevice;
	ID3D11DeviceContext *gDeviceContext;

private:
	IDXGISwapChain *gSwapChain;
	
	ID3D11RenderTargetView *gBackbufferRTV;
	ID3D11DepthStencilView *gDepthStencil;
	ID3D11Buffer *quad;
	ID3D11InputLayout *layout;
	ID3D11VertexShader *vsh;
	ID3D11PixelShader *psh;

	int widht; 
	int height;


	void createShaders();
	void createDepthBuffers();
	HRESULT createDirect3DContext(HWND wndHandle);
	void setViewPort(int width, int height);
	
};
