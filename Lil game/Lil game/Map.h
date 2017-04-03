#pragma once
#include "Camera.h"
#include "Entity.h"
#include <vector>

struct EntityQueryResult {
	Entity *entity;
	float angle;
	float distance;
};

class Map
{
public:
	Map();
	virtual ~Map();

	void add_entity(Entity *entity);
	void update(float dt, Camera *cam);

	std::vector<EntityQueryResult> get_entities_in_radius(Entity *self, float radius);

//private:
	std::vector <Entity*> entitys;
	std::vector <Entity*> entitys_to_add;
};
