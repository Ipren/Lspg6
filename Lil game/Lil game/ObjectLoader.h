#pragma once
#include <vector>
#include <fstream>
#include <d3d11.h>
#include <DirectXMath.h>
using DirectX::XMFLOAT3;

using std::vector;

struct Vertex {
	float x, y, z;
};

struct UV {
	float x, y;
};

struct VertexNormal {
	float x, y, z;
};


class ObjectLoader
{
private:
	//Triangle Vertex array
	XMFLOAT3* tva;
	//File to read
	std::ifstream OBJFile;
	//Vertex array
	vector<Vertex> verts;
	//UV array
	vector<UV> UVs;
	//Vertex Normal array
	vector<VertexNormal> Normals;
	//Index
	vector<WORD> vIndex;
	UINT* ind;
	//For drawing
	ID3D11Buffer* pVertexBuffer = nullptr;
	ID3D11Buffer* pIndexBuffer = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;


public:
	void ObjectLoader::load(std::string filename, ID3D11Device& device, ID3D11DeviceContext& deviceContext, std::vector<XMFLOAT3>& vertexArray, std::vector<UINT>& indexArray);
	

	//void Load(std::string FileName, ID3D11Device& device, ID3D11DeviceContext& deviceContext, XMFLOAT3*& vertexArray, UINT& vaSize, UINT*& indexArray, UINT& iaSize);

	vector<WORD>& GetIndexArray() { return vIndex; }
	void Draw();
	void CreateBuffers();
	int NumberOfVerts() { return verts.size(); }
	ObjectLoader();
	ObjectLoader(std::string FileName, ID3D11Device& device, ID3D11DeviceContext& deviceContext);
	~ObjectLoader();
};

