#pragma once
#include <DirectXMath.h>
#include "Definitions.h"
#include "Mesh.h"
class Map;
using namespace DirectX;

class Entity
{
public:
	Entity();
	Entity(EntityType type, XMFLOAT3 position, XMFLOAT2 velocity, float radius);
	virtual ~Entity();
	void reset_velocity() { this->velocity.x = 0; this->velocity.y = 0; }
	void add_velocity(float x, float y) { this->velocity.x += x; this->velocity.y += y; }
	DirectX::XMFLOAT3 position;
	float radius;

	virtual void update(Map *map, float dt) {}

	//private:
	DirectX::XMFLOAT2 acceleration;
	DirectX::XMFLOAT2 velocity;
	float angle;
	EntityType type;
	bool dead;

	Mesh* pMesh;
private:
};
