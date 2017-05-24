#ifndef MESH_CONTAINER_H
#define MESH_CONTAINER_H

#include "Mesh.h"
#include "SkinnedMesh.h"
#include <fstream>
#include <unordered_map>
class MeshContainer
{
public:
	static std::unordered_map<std::string, Mesh*> staticMeshes;
	static std::unordered_map<std::string, SkinnedMesh*> skinnedMeshes;

	MeshContainer()
	{
		std::ifstream models("models.txt");
	}
	~MeshContainer();

	void Init();
	static ID3D11VertexShader *skinned_vs;
	static ID3D11PixelShader *skinned_ps;
	static ID3D11InputLayout *skinned_layout;

	static ID3D11VertexShader *custom_mesh_vsh;
	static ID3D11PixelShader *custom_mesh_psh;
	static ID3D11InputLayout *custom_mesh_layout;
};
#endif