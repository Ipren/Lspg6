#pragma once
#include <DirectXMath.h>
#include "SoundSingelton.h"

enum class EntityType
{
	Player,
	Spell,
	Wall
};

enum Button
{
	A, B, X, Y,
	Rb, Rt, Lb, Lt,
	Up, Down, Left, Right,
	Lstick, Rstick,
	Start, Back
};

enum GameState {
	MainMenu,
	ChoosePowers,
	Playing,
	UpgradeMenu
};

struct Particle
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 velocity;
	float age;
	int type;
};

struct Emitterlocation
{
	DirectX::XMFLOAT4 randomVector;
	DirectX::XMFLOAT3 position;
	int particleType;
};