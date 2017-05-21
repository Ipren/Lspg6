#include "MeshContainer.h"
#include "Helpers.h"
#include "Globals.h"
#include "dxerr.h"
#include "G6Import.h"

std::unordered_map<std::string, Mesh*> MeshContainer::staticMeshes;
std::unordered_map<std::string, SkinnedMesh*> MeshContainer::skinnedMeshes;


ID3D11VertexShader *MeshContainer::skinned_vs = nullptr;
ID3D11PixelShader *MeshContainer::skinned_ps = nullptr;
ID3D11InputLayout *MeshContainer::skinned_layout = nullptr;

ID3D11VertexShader *MeshContainer::custom_mesh_vsh = nullptr;
ID3D11PixelShader *MeshContainer::custom_mesh_psh = nullptr;
ID3D11InputLayout *MeshContainer::custom_mesh_layout = nullptr;

MeshContainer::~MeshContainer()
{
}

std::string GetFileExtension(const std::string& FileName)
{
	if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of(".") + 1);
	return "";
}

void MeshContainer::Init()
{
	//Load meshes///
	//string path = "Resources/Graphics/" + fileName;
	//std::ifstream file(path);



	SkinnedMesh* mesh = new SkinnedMesh();
	mesh->scale = 0.2f;
	mesh->LoadSkinned("arcane_spell.G6Skin", globalDevice, globalDeviceContext);
	MeshContainer::skinnedMeshes["arcane_spell"] = mesh;

	Mesh* mesh2 = new Mesh();
	mesh2->LoadStatic("arena3.G6Mesh", globalDevice, globalDeviceContext);
	MeshContainer::staticMeshes["arena"] = mesh2;
	//////////////

	///Vertex buffer
	//D3D11_BUFFER_DESC bufferDesc;
	//memset(&bufferDesc, 0, sizeof(bufferDesc));
	//bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.ByteWidth = sizeof(SkinnedVertex) * this->mesh->verts.size();
	//D3D11_SUBRESOURCE_DATA data;
	//data.pSysMem = &mesh->verts[0];
	//globalDevice->CreateBuffer(&bufferDesc, &data, &pVertexBuffer);

	//Create skinned shaders
	{
		ID3DBlob *blob = compile_shader(L"Skinned.hlsl", "VS", "vs_5_0", globalDevice);
		DXCALL(globalDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &skinned_vs));

		//int offset = 0;
		D3D11_INPUT_ELEMENT_DESC input_desc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			//{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		//custom_mesh_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, device);
		HRESULT hr = globalDevice->CreateInputLayout(input_desc, 5, blob->GetBufferPointer(), blob->GetBufferSize(), &this->skinned_layout);
		if (FAILED(hr))
		{
			MessageBox(0, L" input desc creation failed", L"error", MB_OK);
		}

		blob = compile_shader(L"Skinned.hlsl", "PS", "ps_5_0", globalDevice);
		DXCALL(globalDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &skinned_ps));
		blob->Release();
	}

	//Create static mesh shaders
	{
		ID3DBlob *blob = compile_shader(L"Mesh.hlsl", "VS", "vs_5_0", globalDevice);
		DXCALL(globalDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &custom_mesh_vsh));

		//int offset = 0;
		D3D11_INPUT_ELEMENT_DESC input_desc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			//{ "TEXCOORD", 0, DXGI_FORMAT_R16G16_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		//custom_mesh_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, device);
		HRESULT hr = globalDevice->CreateInputLayout(input_desc, ARRAYSIZE(input_desc), blob->GetBufferPointer(), blob->GetBufferSize(), &this->custom_mesh_layout);
		if (FAILED(hr))
		{
			MessageBox(0, L" input desc creation failed", L"error", MB_OK);
		}

		blob = compile_shader(L"Mesh.hlsl", "PS", "ps_5_0", globalDevice);
		DXCALL(globalDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &custom_mesh_psh));
		blob->Release();
	}
	///Constant buffer
	//D3D11_BUFFER_DESC cbDesc;
	//cbDesc.ByteWidth = sizeof(XMMATRIX)*aFinalMatrices.size();
	//cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	//cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//cbDesc.MiscFlags = 0;
	//cbDesc.StructureByteStride = 0;
	//D3D11_SUBRESOURCE_DATA InitData;
	//InitData.pSysMem = &aFinalMatrices[0];
	//InitData.SysMemPitch = 0;
	//InitData.SysMemSlicePitch = 0;
	//hr = this->device->CreateBuffer(&cbDesc, &InitData, &cbJointTransforms);
	//if (FAILED(hr))
	//	MessageBox(0, L"cbJointTransforms buffer creation failed", L"error", MB_OK);
	/////TESTMATRIX BUFFER
	////Constant buffer
	//D3D11_BUFFER_DESC cbTest;
	//cbTest.ByteWidth = sizeof(XMMATRIX);
	//cbTest.Usage = D3D11_USAGE_DYNAMIC;
	//cbTest.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//cbTest.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//cbTest.MiscFlags = 0;
	//cbTest.StructureByteStride = 0;
	//D3D11_SUBRESOURCE_DATA InitTestData;
	//InitTestData.pSysMem = &aFinalMatrices[0];
	//InitTestData.SysMemPitch = 0;
	//InitTestData.SysMemSlicePitch = 0;
	//hr = this->device->CreateBuffer(&cbTest, &InitTestData, &cbTestMatrix);
	//if (FAILED(hr))
	//	MessageBox(0, L"cbJointTransforms buffer creation failed", L"error", MB_OK);
}
