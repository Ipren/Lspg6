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

	SkinnedMesh* mesh = new SkinnedMesh();
	mesh->scale = 0.2f;
	mesh->LoadSkinned("arcane_projectile.G6Skin", globalDevice, globalDeviceContext);
	MeshContainer::skinnedMeshes["arcane_spell"] = mesh;

	mesh = new SkinnedMesh();
	mesh->scale = 1.0f;
	mesh->LoadSkinned("char_1.G6Skin", globalDevice, globalDeviceContext);
	mesh->animations["idle"].m_frameCount = 80;
	mesh->texture = new Texture(L"../Resources/textures/arcanewall.png ");
	MeshContainer::skinnedMeshes["character"] = mesh;

	mesh = new SkinnedMesh();
	mesh->scale = 0.16f;
	mesh->LoadSkinned("arcane_wall.G6Skin", globalDevice, globalDeviceContext);
	mesh->animations["idle"].m_frameCount = 240;
	mesh->texture = new Texture(L"../Resources/textures/arcanewall.png ");
	MeshContainer::skinnedMeshes["arcane_wall"] = mesh;

	mesh = new SkinnedMesh();
	mesh->scale = 0.16f;
	mesh->LoadSkinned("arcane_wall.G6Skin", globalDevice, globalDeviceContext);
	mesh->animations["idle"].m_frameCount = 240;
	mesh->texture = new Texture(L"../Resources/textures/icewall.png ");
	MeshContainer::skinnedMeshes["ice_wall"] = mesh;

	mesh = new SkinnedMesh();
	mesh->scale = 0.59f;
	mesh->LoadSkinned("stone_wall.G6Skin", globalDevice, globalDeviceContext);
	mesh->animations["idle"].m_frameCount = 240;
	mesh->texture = new Texture(L"../Resources/textures/stonewall.png ");
	MeshContainer::skinnedMeshes["earth_wall"] = mesh;

	mesh = new SkinnedMesh();
	mesh->scale = 0.59f;
	mesh->LoadSkinned("stone_wall.G6Skin", globalDevice, globalDeviceContext);
	mesh->animations["idle"].m_frameCount = 240;
	mesh->texture = new Texture(L"../Resources/textures/firewall.png ");
	MeshContainer::skinnedMeshes["fire_wall"] = mesh;

	Mesh* mesh2 = new Mesh();
	mesh2->scale = 0.009f;
	mesh2->LoadStatic("character.G6Mesh", globalDevice, globalDeviceContext);
	MeshContainer::staticMeshes["character"] = mesh2;
	//////////////

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
}
