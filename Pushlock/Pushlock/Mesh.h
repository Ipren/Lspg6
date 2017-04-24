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
	bool LoadStatic(std::string filename, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	bool LoadSKinned(std::string filename, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	///////////////////////
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	///////////////////////
private:
	void CreateBuffers();

	vector<XMFLOAT3> vertexArray;
	vector<UINT> indexArray;
	ID3D11Buffer* pVertexBuffer = nullptr;
	ID3D11Buffer* pIndexBuffer = nullptr;
};

