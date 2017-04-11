#include "Mesh.h"
#include <string>
#include "ObjectLoader.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

bool Mesh::LoadOBJ(std::string filename, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
	ObjectLoader loader(filename, *device, *deviceContext);
	loader.load(filename, *device, *deviceContext, vertexArray, indexArray);
	CreateBuffers();
	return false;
}

void Mesh::Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	if (device == nullptr)
		return;

	UINT32 vertexSize = sizeof(XMFLOAT3);
	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &vertexSize, &offset);
	deviceContext->IASetIndexBuffer(this->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->DrawIndexed(indexArray.size(), 0, 0);
}

void Mesh::CreateBuffers()
{
	//Index buffer
	D3D11_BUFFER_DESC iBufferDesc;
	memset(&iBufferDesc, 0, sizeof(iBufferDesc));
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	iBufferDesc.ByteWidth = sizeof(UINT) * indexArray.size();

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = &indexArray[0];
	device->CreateBuffer(&iBufferDesc, &indexData, &pIndexBuffer);


	//Vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * vertexArray.size();

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &vertexArray[0];
	device->CreateBuffer(&bufferDesc, &data, &pVertexBuffer);
}
