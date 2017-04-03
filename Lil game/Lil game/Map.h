#pragma once
#include "Entity.h"
#include <vector>

class Map
{
public:
	Map();
	virtual ~Map();

//private:
	std::vector <Entity*> entitys;
};
