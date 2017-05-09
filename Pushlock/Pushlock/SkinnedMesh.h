#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "MeshHeader.h"

class SkinnedMesh
{
public:
	SkinnedMesh();
	~SkinnedMesh();
	bool LoadSkinned(std::string filename, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void PreDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	///////////////////////
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	///////////////////////

	void PrepareShaders();
	sSkinnedMesh* Get_sSkinnedMesh() { return mesh; }
private:
	sSkinnedMesh* mesh = nullptr;
	void CreateBuffers();

	ID3D11VertexShader *skinned_vs;
	ID3D11PixelShader *skinned_ps;
	ID3D11InputLayout *skinned_layout;

	ID3D11Buffer* pVertexBuffer;

};

