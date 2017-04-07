#include "Entity.h"
#include "Mesh.h"


Entity::Entity()
{
}

Entity::Entity(EntityType type, XMFLOAT3 position, XMFLOAT2 velocity, float radius)
	: type(type), position(position), acceleration({ 0.f, 0.f }), velocity(velocity), radius(radius), angle(0.f), dead(false)
{
}

Entity::~Entity()
{
	if (this->mesh != nullptr)
		delete mesh;
}

void Entity::DrawMesh()
{
	if (this->mesh != nullptr)
		mesh->Draw();

}

void Entity::AddMesh(std::string filename)
{
	mesh = new Mesh();
	mesh->LoadOBJ(filename);
}
