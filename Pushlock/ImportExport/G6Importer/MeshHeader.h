#pragma once
#include <stdint.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
using std::string;
using std::vector;

struct MeshHeader
{
	uint8_t meshNameLength;
	//name

	uint32_t numberOfVerts;
	//verts

	uint32_t numberOfIndex;
	//index

	uint32_t numberOfUVSets;
	//index
};

struct UVSet
{
	int id;
	int name_length;
	string name;
};

struct Vertex
{
	float posX, posY, posZ;
	float norX, norY, norZ;
	float u, v;
	//uv1
	//uv2
	//uv3
	
	//other stuff?
};

struct UV {
	float U, V;
};

struct Joint {
	int parent_id;
	string name;
};

struct sMesh
{
	MeshHeader header;
	string name;
	vector<Vertex> verts;
	vector<UVSet> uvsets;
	vector<int> indices;
	bool is_skinned;
	uint8_t mat_id;
	vector<UV> uvs;
};