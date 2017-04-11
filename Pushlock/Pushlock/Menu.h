#pragma once
#include <DirectXMath.h>



class Menu
{
public:
	Menu();
	virtual ~Menu();

private:
	struct Button
	{
		DirectX::XMVECTOR quad[4];
	};
	DirectX::XMVECTOR menuQuad[4];
	
};