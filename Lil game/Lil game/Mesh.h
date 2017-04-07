#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
using DirectX::XMFLOAT3;
using std::vector;

class Mesh
{
public:
	Mesh();
	~Mesh();
	bool LoadOBJ(std::string filename);
	void Draw();

	///////////////////////
	static ID3D11Device* device;
	static ID3D11DeviceContext* deviceContext;
	///////////////////////
private:
	void CreateBuffers();

	vector<XMFLOAT3> vertexArray;
	vector<UINT> indexArray;
	ID3D11Buffer* pVertexBuffer = nullptr;
	ID3D11Buffer* pIndexBuffer = nullptr;
};

