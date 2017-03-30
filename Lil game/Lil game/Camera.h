#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "dxerr.h"

#include "Globals.h"

using namespace DirectX;

class Camera
{
public:
	Camera(XMVECTOR pos, XMVECTOR look, ID3D11Device *gDevice);
	~Camera();

	void update(float dt, ID3D11DeviceContext *gDeviceContext);

	struct BufferVals {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	} vals;

	XMVECTOR pos, look;

	ID3D11Buffer *wvp_buffer;
};

