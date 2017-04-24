#pragma once
#include <stdint.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
using std::string;
using std::vector;

//Info about all materials
struct MatHeader {
	uint32_t mat_count;

};


//Info about one material
struct MatSubHeader {
	uint8_t matNameLength;
	//name
};

//Fixed size material data
struct sMaterialData {
	uint32_t diffuse;
	uint32_t specular;

	uint8_t diffusePathLength;
};

//Material "object"
struct sMaterial
{
	MatSubHeader subheader;
	string name;
	string diffuse_path;

	sMaterialData data;
};
