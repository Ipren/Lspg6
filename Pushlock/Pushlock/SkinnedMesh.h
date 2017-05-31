#pragma once
#include <d3d11.h>
#include "Texture.h"
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "MeshHeader.h"
#include <unordered_map>

class SkinnedMesh
{
public:
	SkinnedMesh();
	~SkinnedMesh();
	bool LoadSkinned(std::string filename, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void PreDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext);	// ?
	void Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext);	// ?

																			///////////////////////

	Texture* texture = nullptr;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	///////////////////////

	void PrepareShaders();
	sSkinnedMesh* Get_sSkinnedMesh() { return mesh; }


	sSkinnedMesh* mesh = nullptr;
	std::unordered_map<string, AnimationClip> animations;
	void CreateBuffers();

	ID3D11Buffer* pVertexBuffer = nullptr;
	ID3D11Buffer* pIndexBuffer = nullptr;

	ID3D11VertexShader* skinned_vs;
	ID3D11PixelShader* skinned_ps;
	ID3D11InputLayout* skinned_layout;

	float scale = 1.0f;
};