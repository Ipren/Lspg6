#pragma once

#include <vector>

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

	void focus(std::vector<XMVECTOR> positions);
	void update(float dt, ID3D11DeviceContext *gDeviceContext);
	void updateCamPosBuffer(ID3D11Buffer *rBuffer);

	__declspec(align(16))
	struct BufferVals {
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMMATRIX normal;
	};

	float znear, zfar;

	BufferVals vals;

	XMVECTOR pos, look;
	XMVECTOR target;

	XMVECTOR temp;
	XMVECTOR offset;

	ID3D11Buffer *wvp_buffer;
	ID3D11Buffer *floatwvpBuffer;

	struct testMatrix {
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 proj;
	} wvpmatrixes;
};

