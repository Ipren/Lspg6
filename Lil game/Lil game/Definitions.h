#pragma once


enum class EntityType
{
	Player,
	Spell
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