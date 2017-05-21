#pragma once
#include "MeshHeader.h"
#include "MatHeader.h"
#include <fstream>
class G6Import
{
public:
	static bool ImportStaticMesh(const char * filename, sMesh* outMesh, vector<sMaterial*>& outMaterials);
	static bool ImportAnimatedMesh(const char * filename, sSkinnedMesh* outMesh);

	G6Import();
	~G6Import();
};

