#pragma once

#include <d3d11.h>

class Camera;

#define WIDTH 1280
#define HEIGHT 800

extern IDXGISwapChain *gSwapChain;
extern ID3D11Device *gDevice;
extern ID3D11DeviceContext *gDeviceContext;
extern ID3D11RenderTargetView *gBackbufferRTV;
extern ID3D11DepthStencilView *gDepthStencil;

extern Camera *gCamera;