#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "dxerr.h"

#include "Globals.h"

using namespace DirectX;

class Camera
{
public:
	Camera(XMVECTOR pos, XMVECTOR look);
	~Camera();

	void update(float dt);

	struct BufferVals {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
	} vals;

	XMVECTOR pos, look;

	ID3D11Buffer *wvp_buffer;
};

