#include "SkinnedMesh.h"
#include "G6Import.h"
#include "Globals.h"


SkinnedMesh::SkinnedMesh()
{
	this->device = globalDevice;
	this->deviceContext = globalDeviceContext;
}


SkinnedMesh::~SkinnedMesh()
{
}

bool SkinnedMesh::LoadSkinned(std::string filename, ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
	//ObjectLoader loader(filename, *device, *deviceContext);
	//loader.load(filename, *device, *deviceContext, vertexArray, indexArray);
	this->mesh = new sSkinnedMesh;
	vector<sMaterial*> materials;
	G6Import::ImportAnimatedMesh(filename.c_str(), mesh);
	this->animations["idle"] = mesh->animation;



	CreateBuffers();

	//this->vertexArray.clear();
	//this->vertexArray.shrink_to_fit();

	return false;
}

void SkinnedMesh::PreDraw(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	deviceContext->VSSetShader(skinned_vs, nullptr, 0);
	deviceContext->PSSetShader(skinned_ps, nullptr, 0);
	if (device == nullptr)
		return;

	UINT32 vertexSize = sizeof(SkinnedVertex);
	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &vertexSize, &offset);
	deviceContext->IASetInputLayout(this->skinned_layout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (pIndexBuffer != nullptr)
		deviceContext->IASetIndexBuffer(this->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void SkinnedMesh::CreateBuffers()
{
}