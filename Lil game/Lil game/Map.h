#pragma once
#include "Camera.h"
#include "Entity.h"
#include <vector>

class Map
{
public:
	Map();
	virtual ~Map();

	void update(float dt, Camera *cam);

//private:
	std::vector <Entity*> entitys;
};
