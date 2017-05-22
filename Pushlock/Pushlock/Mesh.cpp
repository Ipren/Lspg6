#include "Mesh.h"
#include <string>
//#include "ObjectLoader.h"
#include "G6Import.h"

#include "Helpers.h"
#include "dxerr.h"

Mesh::Mesh()
{
	custom_mesh_vsh = nullptr;
	custom_mesh_vsh = nullptr;
}

Mesh::~Mesh()
{
	if (this->mesh != nullptr)
		delete mesh;
}

bool Mesh::LoadStatic(std::string filename, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
	//ObjectLoader loader(filename, *device, *deviceContext);
	//loader.load(filename, *device, *deviceContext, vertexArray, indexArray);
	this->mesh = new sMesh;
	vector<sMaterial*> materials;
	vector<sLight*> lights;
	G6Import::ImportStaticMesh(filename.c_str(), mesh, materials, lights);


	for (auto& v : mesh->verts) {
		this->vertexArray.push_back(XMFLOAT3(v.posX, v.posY, v.posZ));
		this->vertexArray.push_back(XMFLOAT3(v.norX, v.norY, v.norZ));
	}
	this->vertexCount = vertexArray.size();



	CreateBuffers();

	//this->vertexArray.clear();
	//this->vertexArray.shrink_to_fit();

	return false;
}

void Mesh::PreDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
	deviceContext->VSSetShader(custom_mesh_vsh, nullptr, 0);
	deviceContext->PSSetShader(custom_mesh_psh, nullptr, 0);
	if (device == nullptr)
		return;

	UINT32 vertexSize = sizeof(XMFLOAT3) * 2;
	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &vertexSize, &offset);
	deviceContext->IASetInputLayout(this->custom_mesh_layout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (pIndexBuffer != nullptr)
		deviceContext->IASetIndexBuffer(this->pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{






	if (pIndexBuffer != nullptr)
		deviceContext->DrawIndexed(indexArray.size(), 0, 0);
	else
		deviceContext->Draw(this->vertexCount / 2, 0);
}

void Mesh::CreateBuffers()
{

	//Index buffer
	//D3D11_BUFFER_DESC iBufferDesc;
	//memset(&iBufferDesc, 0, sizeof(iBufferDesc));
	//iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//iBufferDesc.ByteWidth = sizeof(UINT) * indexArray.size();

	//D3D11_SUBRESOURCE_DATA indexData;
	//indexData.pSysMem = &indexArray[0];
	//device->CreateBuffer(&iBufferDesc, &indexData, &pIndexBuffer);


	//Vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(XMFLOAT3) * vertexArray.size();

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &vertexArray[0];
	device->CreateBuffer(&bufferDesc, &data, &pVertexBuffer);




	ID3DBlob *blob = compile_shader(L"Mesh.hlsl", "VS", "vs_5_0", device);
	DXCALL(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &custom_mesh_vsh));

	//int offset = 0;
	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		//{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	//custom_mesh_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, device);
	HRESULT hr = this->device->CreateInputLayout(input_desc, ARRAYSIZE(input_desc), blob->GetBufferPointer(), blob->GetBufferSize(), &this->custom_mesh_layout);
	if (FAILED(hr))
	{
		MessageBox(0, L" input desc creation failed", L"error", MB_OK);
	}

	blob = compile_shader(L"Mesh.hlsl", "PS", "ps_5_0", device);
	DXCALL(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &custom_mesh_psh));
	blob->Release();
}

void Mesh::PrepareShaders()
{
	deviceContext->IASetInputLayout(custom_mesh_layout);
}
