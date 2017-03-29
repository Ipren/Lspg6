#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX proj;
};

