#include "ObjectLoader.h"
#include <iostream>
#include <string>
using std::vector;
void ObjectLoader::load(std::string filename, ID3D11Device& device, ID3D11DeviceContext& deviceContext, std::vector<XMFLOAT3>& vertexArray, std::vector<UINT>& indexArray)
{

	std::ifstream file;
	file.open(filename);

	if (file.is_open())
	{
		UINT nOfVerts = 0;
		UINT nOfIndex = 0;

		std::string input;
		while (!file.eof())
		{
			file >> input;
			if (input == "v")
			{
				XMFLOAT3 v;
				file >> v.x >> v.y >> v.z;
				vertexArray.push_back(v);
			}
			else if (input == "f")
			{
				UINT num1, num2, num3;
				std::string garbage;
				file >> num1 >> garbage >> num2 >> garbage >> num3 >> garbage;

				indexArray.push_back(num1 - 1);
				indexArray.push_back(num2 - 1);
				indexArray.push_back(num3 - 1);
			}

		}
		file.close();
	}
}

//void ObjectLoader::Load(std::string FileName, ID3D11Device& device, ID3D11DeviceContext& deviceContext, XMFLOAT3 vertexArray[], UINT indexArray[])
//{
//	this->pDevice = &device;
//	this->pDeviceContext = &deviceContext;
//
//	//Open file
//	OBJFile.open(FileName);
//	
//	
//	//Parse vertex, UV and Normal information from the file
//	if (OBJFile.is_open())
//	{
//		std::string input;
//		while (!OBJFile.eof())
//		{
//			OBJFile >> input;
//			if (input == "v")
//			{
//				Vertex v;
//				OBJFile >> v.x >> v.y >> v.z;
//				this->verts.push_back(v);
//			}
//			else if (input == "vt")
//			{
//				UV v;
//				OBJFile >> v.x >> v.y;
//				this->UVs.push_back(v);
//			}
//			else if (input == "vn")
//			{
//				VertexNormal v;
//				OBJFile >> v.x >> v.y >> v.z;
//				this->Normals.push_back(v);
//			}
//			else if (input == "f")
//			{
//				DWORD num1, num2, num3;
//				std::string garbage;
//				OBJFile >> num1 >> garbage >> num2 >> garbage >> num3 >> garbage;
//
//				vIndex.push_back(num1-1);
//				vIndex.push_back(num2-1);
//				vIndex.push_back(num3-1);
//			}
//			
//		}
//	}
//	//new index array
//	ind = new UINT[vIndex.size()];
//	for (int i = 0; i < vIndex.size(); i++)
//		ind[i] = vIndex[i];
//	//close file
//	OBJFile.close();
//
//	//Allocate array of TriangleVertex
//	tva = new XMFLOAT3[this->NumberOfVerts()];
//
//	//Fill array with Vertex and UV information
//	for (int i = 0; i < NumberOfVerts(); i++)
//	{
//		tva[i].x = verts[i].x;
//		tva[i].y = verts[i].y;
//		tva[i].z = verts[i].z;
//	}
//
//	/*std::cout << std::to_string(this->verts[0].x) + " " + std::to_string(this->verts[0].y) + " " + std::to_string(this->verts[0].z) << std::endl;
//	*/
//	//return pointer to array
//	this->CreateBuffers();
//}

//
//TriangleVertex* ObjectLoader::Load(std::string FileName)
//{
//	//Open file
//	OBJFile.open(FileName);
//
//
//	//Parse vertex, UV and Normal information from the file
//	if (OBJFile.is_open())
//	{
//		std::string input;
//		while (!OBJFile.eof())
//		{
//			OBJFile >> input;
//			if (input == "v")
//			{
//				Vertex v;
//				OBJFile >> v.x >> v.y >> v.z;
//				this->verts.push_back(v);
//			}
//			else if (input == "vt")
//			{
//				UV v;
//				OBJFile >> v.x >> v.y;
//				this->UVs.push_back(v);
//			}
//			else if (input == "vn")
//			{
//				VertexNormal v;
//				OBJFile >> v.x >> v.y >> v.z;
//				this->Normals.push_back(v);
//			}
//			else if (input == "f")
//			{
//
//				int num1, num2, num3;
//				std::string garbage;
//				OBJFile >> num1 >> garbage >> num2 >> garbage >> num3 >> garbage;
//
//				vIndex.push_back(num1);
//				vIndex.push_back(num2);
//				vIndex.push_back(num3);
//			}
//
//		}
//		OBJFile.close();
//	}
//
//	//close file
//
//	//Allocate array of TriangleVertex
//	tva = new TriangleVertex[this->NumberOfVerts()];
//
//	//Fill array with Vertex and UV information
//	for (int i = 0; i < NumberOfVerts(); i++)
//	{
//		tva[i].x = verts[i].x;
//		tva[i].y = verts[i].y;
//		tva[i].z = verts[i].z;
//
//		tva[i].U = UVs[i].x;
//		tva[i].V = UVs[i].y;
//	}
//
//	/*std::cout << std::to_string(this->verts[0].x) + " " + std::to_string(this->verts[0].y) + " " + std::to_string(this->verts[0].z) << std::endl;
//	*/
//	//return pointer to array
//	this->CreateBuffers();
//	return tva;
//}

void ObjectLoader::Draw()
{
	if (pDevice == nullptr)
		return;

	UINT32 vertexSize = sizeof(float) * 5;
	UINT32 offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &vertexSize, &offset);
	pDeviceContext->IASetIndexBuffer(this->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDeviceContext->DrawIndexed(vIndex.size(),0 , 0);
	//pDeviceContext->Draw(this->NumberOfVerts(), 0);

}

void ObjectLoader::CreateBuffers()
{
	//Index buffer
	D3D11_BUFFER_DESC iBufferDesc;
	memset(&iBufferDesc, 0, sizeof(iBufferDesc));
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	iBufferDesc.ByteWidth = sizeof(DWORD) * (vIndex.size()-3);

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = ind;
	pDevice->CreateBuffer(&iBufferDesc, &indexData, &pIndexBuffer);
	

	//Vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * this->NumberOfVerts();

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = tva;
	pDevice->CreateBuffer(&bufferDesc, &data, &pVertexBuffer);
}





ObjectLoader::ObjectLoader()
{
}

ObjectLoader::ObjectLoader(std::string FileName, ID3D11Device& device, ID3D11DeviceContext& deviceContext)
{
	
}

ObjectLoader::~ObjectLoader()
{
	//Delete array
	delete[] tva;
	delete[] ind;
}
