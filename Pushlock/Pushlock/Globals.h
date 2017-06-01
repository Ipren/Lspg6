#pragma once

#include <d3d11.h>

#include "Gamepad.h"

class Camera;

#define RECONN_TIME 5.0
#define WIDTH 1920
#define HEIGHT 1080

extern IDXGISwapChain *gSwapChain;
extern ID3D11Device *globalDevice;//use the one in renderer
extern ID3D11DeviceContext *globalDeviceContext;
extern ID3D11RenderTargetView *gBackbufferRTV;
extern ID3D11DepthStencilView *gDepthStencil;

extern Camera *gCamera;
extern Gamepad *gGamepads[4];

