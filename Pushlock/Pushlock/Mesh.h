#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "MeshHeader.h"

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using std::vector;

class Mesh
{
public:
	Mesh();
	~Mesh();
	bool LoadStatic(std::string filename, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void PreDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	///////////////////////
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	///////////////////////

	void PrepareShaders();
private:
	sMesh* mesh = nullptr;
	void CreateBuffers();

	vector<XMFLOAT3> vertexArray;
	vector<XMFLOAT2> uvArray;

	UINT vertexCount;
	vector<UINT> indexArray;
	ID3D11Buffer* pVertexBuffer = nullptr;
	ID3D11Buffer* pIndexBuffer = nullptr;

	ID3D11VertexShader *custom_mesh_vsh;
	ID3D11PixelShader *custom_mesh_psh;
	ID3D11InputLayout *custom_mesh_layout;
};
