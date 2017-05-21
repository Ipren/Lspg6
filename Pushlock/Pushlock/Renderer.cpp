#include "Renderer.h"
#include "Spell.h"
#include "Player.h"
#include <cstdlib>
#include <time.h>
#include "Constants.h"
#include "Globals.h"

ParticleSystem *FXSystem;

const UINT startParticleCount = 0;
//makes it so the actual amount of particles in the buffers is used
const UINT UAVFLAG = -1;

ID3D11Device *globalDevice;
ID3D11DeviceContext *globalDeviceContext;


static ID3D11RenderTargetView *RESET_RTV[16] = {};
static ID3D11ShaderResourceView *RESET_SRV[16] = {};

Renderer::Renderer()
{

}

Renderer::Renderer(HWND wndHandle, int width, int height)
{
	srand(static_cast <unsigned> (time(0)));
	this->gBackbufferRTV = nullptr;
	this->DepthBufferMS = nullptr;
	this->gDevice = nullptr;
	this->gDeviceContext = nullptr;
	this->gSwapChain = nullptr;

	this->debug_map_layout = nullptr;
	this->debug_map_psh = nullptr;
	this->debug_map_quad = nullptr;
	this->debug_map_vsh = nullptr;

	this->mesh_vsh = nullptr;
	this->mesh_psh = nullptr;

	this->debug_entity_circle = nullptr;
	this->debug_entity_layout = nullptr;
	this->debug_entity_psh = nullptr;
	this->debug_entity_vsh = nullptr;

	this->mesh_psh = nullptr;
	this->mesh_psh = nullptr;

	this->dLightBuffer = nullptr;

	this->nullSRV = nullptr;
	this->nullUAV = nullptr;
	this->nullRTV = nullptr;
	this->gb = true;

	this->height = height;
	this->width = width;
	this->totalTime = 0.0f;
	this->lastParticleInsert = 0.0f;
	this->emitterCount = 0;
	this->toatlShrunkAmount = 0.0f;
	this->heatHazeCounter = 0.0f;

	this->createDirect3DContext(wndHandle);
	this->createDepthBuffers();
	this->createBlurPass();
	this->createShaders();
	this->setViewPort(width, height);
	this->createParticleBuffer(524288);
	this->createLightBuffers();
	this->createCameraBuffer();
	this->createcpMenuShaders();
	this->createFullScreenQuad();
	this->createScoreQuad();
	this->createShadowMap();
	this->loadTexture();

	this->createCooldownBuffers(); //Move these so they render before the models
	this->createCooldownShaders(); //Move these so they render before the models

	this->createHPBuffers();
	this->createHPShaders();

	this->createCuBuffers();
	this->createCUShaders();
	this->createMapResurces();

	FXSystem = new ParticleSystem(L"../Resources/Particles.no", 4096, WIDTH, HEIGHT, gDevice, gDeviceContext);

	HRESULT hr = this->gDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void **>(&debugDevice));
	if (FAILED(hr))
	{
		MessageBox(0, L"debug device failed", L"error", MB_OK);
	}
	globalDevice = this->gDevice;
	globalDeviceContext = this->gDeviceContext;


	this->create_debug_entity();

	mapmesh = new Mesh();
	mapmesh->LoadStatic("arena3.G6Mesh", globalDevice, globalDeviceContext);
}

Renderer::~Renderer()
{
	ULONG test = 0;
	this->gDeviceContext->ClearState();
	this->gBackbufferRTV->Release();
	this->DepthBufferMS->Release();
	this->gDevice->Release();
	this->gDeviceContext->Release();
	this->gSwapChain->Release();
	this->debug_map_layout->Release();
	this->debug_map_quad->Release();
	this->debug_map_psh->Release();
	this->debug_map_vsh->Release();

	if (this->mesh_vsh)
		this->mesh_vsh->Release();
	if (this->mesh_psh)
	this->mesh_psh->Release();

	this->color_buffer->Release();
	this->debug_entity_circle->Release();
	this->debug_entity_layout->Release();
	this->debug_entity_psh->Release();
	this->debug_entity_vsh->Release();

	if (this->mesh_vsh)
		this->mesh_vsh->Release();
	if (this->mesh_psh)
		this->mesh_psh->Release();

	this->UAVS[0]->Release();
	this->UAVS[1]->Release();
	this->SRVS[0]->Release();
	this->SRVS[1]->Release();
	this->inderectArgumentBuffer->Release();
	this->ParticleCount->Release();
	this->computeShader->Release();
	this->inserter->Release();
	this->pVertex->Release();
	this->pGeometry->Release();
	this->pPixel->Release();
	this->deltaTimeBuffer->Release();
	this->eLocations->Release();
	this->emitterCountBuffer->Release();
	this->randomVecBufer->Release();
	this->stompParticles->Release();
	this->playerPosBuffer->Release();
	this->dLightBuffer->Release();
	this->pLightBuffer->Release();
	this->pLightSRV->Release();
	this->pointLightCountBuffer->Release();
	this->quadVertexBuffer->Release();
	this->roundVertexBuffer->Release();
	this->cpMenuTexture->Release();
	this->cpMenuVs->Release();
	this->cpQuadLayout->Release();
	this->cpmenuPS->Release();
	this->cooldownBuffer->Release();
	this->cooldownCirclesLayout->Release();
	this->cooldownCircles->Release();
	this->cooldownVS->Release();
	this->cooldownPS->Release();
	this->HPVertexBuffer->Release();
	this->HPInputLayout->Release();
	this->HPVS->Release();
	this->HPPS->Release();
	this->cuVertexBuffer->Release();
	this->cuVS->Release();
	this->cuLayout->Release();
	this->cuPS->Release();

	this->mapLayout->Release();
	this->MapPS->Release();
	this->MapVS->Release();
	this->mapVBuffer->Release();
	this->shrinkBuffer->Release();
	this->mapTexture->Release();
	this->mapBlendState->Release();

	this->lavaBuffer->Release();
	this->lavaTexture->Release();
	this->lavaVS->Release();
	this->lavaPS->Release();
	this->heatHazeBuffer->Release();

	/*this->debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);*/
	this->debugDevice->Release();
}

void Renderer::createBlurPass()
{
	D3D11_SAMPLER_DESC sadesc;
	ZeroMemory(&sadesc, sizeof(sadesc));
	sadesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sadesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sadesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sadesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sadesc.MaxLOD = 11.f;
	DXCALL(gDevice->CreateSamplerState(&sadesc, &blur_fs_sampler));

	D3D11_TEXTURE2D_DESC tex_desc;
	ZeroMemory(&tex_desc, sizeof(tex_desc));
	tex_desc.Width = WIDTH;
	tex_desc.Height = HEIGHT;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.MipLevels = 11;
	tex_desc.ArraySize = 1;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
	tex_desc.CPUAccessFlags = 0;
	tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
	ZeroMemory(&sdesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	sdesc.Texture2D.MipLevels = 11;
	sdesc.Texture2D.MostDetailedMip = 0;

	D3D11_RENDER_TARGET_VIEW_DESC rdesc;
	ZeroMemory(&rdesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rdesc.Texture2D.MipSlice = 0;
	
	ID3D11Texture2D *blurtex[2];
	for (int i = 0; i < 2; ++i) {
		DXCALL(gDevice->CreateTexture2D(&tex_desc, nullptr, &blurtex[i]));

		DXCALL(gDevice->CreateShaderResourceView(blurtex[i], &sdesc, &blur_srv[i]));
		DXCALL(gDevice->CreateRenderTargetView(blurtex[i], &rdesc, &blur_rtv[i]));
		gDeviceContext->GenerateMips(blur_srv[i]);
	}
	
	float vertices[] = {
		-1,  1, 0, 0,
		1, -1, 1, 1,
		-1, -1, 0, 1,

		1, -1, 1, 1,
		-1,  1, 0, 0,
		1,  1, 1, 0
	};

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = (UINT)(sizeof(vertices));
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &vertices[0];

	DXCALL(gDevice->CreateBuffer(&desc, &data, &blur_fs_vertices));

	ID3DBlob *blob = compile_shader(L"GaussianPass.hlsl", "VS", "vs_5_0", gDevice);
	DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &blur_fs_vs));

	D3D11_INPUT_ELEMENT_DESC iinput_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	blur_fs_layout = create_input_layout(iinput_desc, ARRAYSIZE(iinput_desc), blob, gDevice);

	blob = compile_shader(L"GaussianPass.hlsl", "GaussianX", "ps_5_0", gDevice);
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &gaussian_x_ps));
	blob = compile_shader(L"GaussianPass.hlsl", "GaussianY", "ps_5_0", gDevice);
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &gaussian_y_ps));

	blob = compile_shader(L"GlowComposite.hlsl", "PS", "ps_5_0", gDevice);
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &blur_composite));
}

void Renderer::create_debug_entity()
{

	std::vector<XMFLOAT3> vertices;
	for (int i = 0; i < 128; ++i)
	{
		XMFLOAT3 vert = {
			sin(2 * XM_PI * i / 128.f),
			0.01f,
			cos(2 * XM_PI * i / 128.f)
		};
		vertices.push_back(vert);
	}

	XMFLOAT3 start = vertices[0];
	start.z += 2.4f;

	vertices[0] = start;
	vertices.push_back(start);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = (UINT)(sizeof(XMFLOAT3) * vertices.size());
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &vertices[0];

	DXCALL(gDevice->CreateBuffer(&desc, &data, &debug_entity_circle));

	ID3DBlob *blob = compile_shader(L"Debug.hlsl", "VS", "vs_5_0", gDevice);
	DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &debug_entity_vsh));

	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	debug_entity_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, gDevice);

	blob = compile_shader(L"Debug.hlsl", "PS", "ps_5_0", gDevice);
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &debug_entity_psh));
}

void Renderer::createShadowMap()
{
	directionalLightPos = { -5, 8, -5 };

	D3D11_RASTERIZER_DESC state;
	ZeroMemory(&state, sizeof(D3D11_RASTERIZER_DESC));
	state.FillMode = D3D11_FILL_SOLID;
	state.CullMode = D3D11_CULL_FRONT;
	state.FrontCounterClockwise = false;
	state.DepthBias = 0;
	state.DepthBiasClamp = 0;
	state.SlopeScaledDepthBias = 0;
	state.DepthClipEnable = true;
	state.ScissorEnable = false;
	state.MultisampleEnable = false;
	state.AntialiasedLineEnable = false;

	DXCALL(gDevice->CreateRasterizerState(&state, &ShadowRaster));

	gDeviceContext->RSGetState(&DefaultRaster);
	
	// TODO: ta bort
	ID3D11Texture2D* shadow_map = NULL;
	D3D11_TEXTURE2D_DESC map_desc;
	map_desc.Width = WIDTH;
	map_desc.Height = HEIGHT;
	map_desc.MipLevels = 1;
	map_desc.ArraySize = 1;
	map_desc.Format = DXGI_FORMAT_R32_FLOAT;
	map_desc.SampleDesc.Count = 1;
	map_desc.SampleDesc.Quality = 0;
	map_desc.Usage = D3D11_USAGE_DEFAULT;
	map_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	map_desc.CPUAccessFlags = 0;
	map_desc.MiscFlags = 0;
	DXCALL(gDevice->CreateTexture2D(&map_desc, NULL, &shadow_map));

	D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
	ZeroMemory(&sdesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sdesc.Format = DXGI_FORMAT_R32_FLOAT;
	sdesc.Texture2D.MipLevels = 1;
	sdesc.Texture2D.MostDetailedMip = 0;

	D3D11_RENDER_TARGET_VIEW_DESC rdesc;
	ZeroMemory(&sdesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rdesc.Format = DXGI_FORMAT_R32_FLOAT;
	rdesc.Texture2D.MipSlice = 0;

	DXCALL(gDevice->CreateShaderResourceView(shadow_map, nullptr, &shadowMapSRV));
	DXCALL(gDevice->CreateRenderTargetView(shadow_map, nullptr, &shadowMapRTV));


	D3D11_SAMPLER_DESC sampdesc;
	ZeroMemory(&sampdesc, sizeof(sampdesc));
	sampdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampdesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampdesc.MaxAnisotropy = 8;
	//sampdesc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;
	DXCALL(gDevice->CreateSamplerState(&sampdesc, &shadowMapSampler));

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(Camera::BufferVals);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &shadow_camera;

	DXCALL(gDevice->CreateBuffer(&desc, &data, &shadow_wvp_buffer));
	
	ID3DBlob *blob = compile_shader(L"Shadow.hlsl", "VS", "vs_5_0", gDevice);
	DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shadowMapVS));

	/*D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	debug_entity_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, gDevice);*/

	blob = compile_shader(L"Shadow.hlsl", "PS", "ps_5_0", gDevice);
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shadowMapPS));
}

void Renderer::createShaders()
{
	/*float vertices[] = {
		-10, 0, -10,
		-10, 0,  10,
		10, 0,  10,

		-10, 0, -10,
		10, 0,  10,
		10, 0, -10
	};*/

	std::vector<XMFLOAT3> vertices;
	
	
	for (int i = 0; i < 128; i++)
	{
		
		XMFLOAT3 vert = {
			sin(2 * XM_PI * i / 128.f) * 15,
			0.01f,
			cos(2 * XM_PI * i / 128.f) * 15
		};

		XMFLOAT3 vert2 = {
			sin(2 * XM_PI * (i+1) / 128.f) * 15,
			0.01f,
			cos(2 * XM_PI * (i+1) / 128.f) * 15
		};
		vertices.push_back({ 0.f, 0.f, 0.f });
		vertices.push_back(vert);
		vertices.push_back(vert2);

	}

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = (UINT)(vertices.size() * 3 * sizeof(float));
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &vertices[0];

	DXCALL(gDevice->CreateBuffer(&desc, &data, &debug_map_quad));

	
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(float) * 4;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	DXCALL(gDevice->CreateBuffer(&desc, nullptr, &color_buffer));
	

	ID3DBlob *blob = compile_shader(L"Simple.hlsl", "VS", "vs_5_0", this->gDevice);
	DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &debug_map_vsh));

	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	debug_map_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, this->gDevice);

	blob = compile_shader(L"Simple.hlsl", "PS", "ps_5_0", this->gDevice);
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &debug_map_psh));
	blob->Release();

	this->createParticleShaders();
}

void Renderer::createDepthBuffers()
{
	ID3D11Texture2D* depth_tex_ms = NULL;
	ID3D11Texture2D* depth_tex = NULL;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = WIDTH;
	descDepth.Height = HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	DXCALL(gDevice->CreateTexture2D(&descDepth, NULL, &depth_tex_ms));
	descDepth.Width = 2048;
	descDepth.Height = 2048;
	descDepth.SampleDesc.Count = 1;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

	DXCALL(gDevice->CreateTexture2D(&descDepth, NULL, &depth_tex));

	descDepth.Width = WIDTH;
	descDepth.Height = HEIGHT;

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	DXCALL(gDevice->CreateDepthStencilState(&dsDesc, &DepthStateReadWrite));
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DXCALL(gDevice->CreateDepthStencilState(&dsDesc, &DepthStateRead));
	dsDesc.DepthEnable = false;
	DXCALL(gDevice->CreateDepthStencilState(&dsDesc, &DepthStateDisable));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	ZeroMemory(&descSRV, sizeof(descSRV));
	descSRV.Format = DXGI_FORMAT_R32_FLOAT;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2D.MipLevels = 1;
	descSRV.Texture2D.MostDetailedMip = 0;
	DXCALL(gDevice->CreateDepthStencilView(depth_tex_ms, &descDSV, &DepthBufferMS));
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DXCALL(gDevice->CreateDepthStencilView(depth_tex, &descDSV, &DepthBuffer));
	DXCALL(gDevice->CreateShaderResourceView(depth_tex, &descSRV, &DepthBufferSRV));
	depth_tex_ms->Release();
	depth_tex->Release();

}

HRESULT Renderer::createDirect3DContext(HWND wndHandle)
{
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = wndHandle;
	scd.SampleDesc.Count = 1;
	scd.Windowed = true;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr))
	{
		ID3D11Texture2D* pBackBuffer = nullptr;
		DXCALL(gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));

		DXCALL(gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV));
		pBackBuffer->Release();

		this->createDepthBuffers();

		ID3D11Texture2D *dtex;
		D3D11_TEXTURE2D_DESC rtv_desc;
		ZeroMemory(&rtv_desc, sizeof(rtv_desc));
		rtv_desc.Width = WIDTH;
		rtv_desc.Height = HEIGHT;
		rtv_desc.Usage = D3D11_USAGE_DEFAULT;
		rtv_desc.MipLevels = 1;
		rtv_desc.ArraySize = 1;
		rtv_desc.SampleDesc.Count = 1;
		rtv_desc.SampleDesc.Quality = 0;
		rtv_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtv_desc.CPUAccessFlags = 0;
		rtv_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		rtv_desc.MiscFlags = 0;

		DXCALL(gDevice->CreateTexture2D(&rtv_desc, nullptr, &dtex));

		DXCALL(gDevice->CreateShaderResourceView(dtex, nullptr, &default_srv));
		DXCALL(gDevice->CreateRenderTargetView(dtex, nullptr, &default_rtv));
	}

	return hr;
}

void Renderer::setViewPort(int width, int height)
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vp);
}

void Renderer::createParticleBuffer(int nrOfParticles)
{
	Particle *particles = new Particle[nrOfParticles];

	for (size_t i = 0; i < nrOfParticles; i++)
	{
		particles[i].position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].age = 0.0f;
		particles[i].type = 0;
	}
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = particles;

	ID3D11Buffer *particleBuffer1 = nullptr;
	ID3D11Buffer *particleBuffer2 = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.ByteWidth = nrOfParticles * sizeof(Particle);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(Particle);
	desc.Usage = D3D11_USAGE_DEFAULT;

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &particleBuffer1);
	if (FAILED(hr))
	{
		MessageBox(0, L"paricle buffer 1 failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateBuffer(&desc, &data, &particleBuffer2);
	if (FAILED(hr))
	{
		MessageBox(0, L"paricle buffer 2 failed", L"error", MB_OK);
	}


	D3D11_BUFFER_UAV uav;
	uav.FirstElement = 0;
	uav.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	uav.NumElements = nrOfParticles;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer = uav;

	hr = this->gDevice->CreateUnorderedAccessView(particleBuffer1, &uavDesc, &this->UAVS[0]);
	if (FAILED(hr))
	{
		MessageBox(0, L"uav 1 failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateUnorderedAccessView(particleBuffer2, &uavDesc, &this->UAVS[1]);
	if (FAILED(hr))
	{
		MessageBox(0, L"uav 2 failed", L"error", MB_OK);
	}

	D3D11_BUFFER_SRV srv;
	srv.FirstElement = 0;
	srv.NumElements = nrOfParticles;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer = srv;

	hr = this->gDevice->CreateShaderResourceView(particleBuffer1, &srvDesc, &this->SRVS[0]);
	if (FAILED(hr))
	{
		MessageBox(0, L"srv 1 failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateShaderResourceView(particleBuffer2, &srvDesc, &this->SRVS[1]);
	if (FAILED(hr))
	{
		MessageBox(0, L"srv 2 failed", L"error", MB_OK);
	}
	particleBuffer1->Release();
	particleBuffer2->Release();

	UINT* init = new UINT[5];
	for (size_t i = 0; i < 5; i++)
	{
		init[i] = 0;
	}

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = 4 * sizeof(UINT);
	desc.Usage = D3D11_USAGE_DEFAULT;

	data.pSysMem = init;
	hr = this->gDevice->CreateBuffer(&desc, &data, &this->ParticleCount);
	if (FAILED(hr))
	{
		MessageBox(0, L"particle count cbuffer creation failed", L"error", MB_OK);
	}

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = 5 * sizeof(UINT);
	desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	desc.Usage = D3D11_USAGE_DEFAULT;

	init[1] = 1;

	hr = this->gDevice->CreateBuffer(&desc, &data, &this->inderectArgumentBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"ind. arg. cbuffer creation failed", L"error", MB_OK);
	}

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = 4 * sizeof(float);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	float temp = 0.0f;

	data.pSysMem = &temp;

	hr = this->gDevice->CreateBuffer(&desc, &data, &this->deltaTimeBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"d time cbuffer creation failed", L"error", MB_OK);
	}


	desc.ByteWidth = 4 * sizeof(int);
	data.pSysMem = &this->emitterCount;

	hr = this->gDevice->CreateBuffer(&desc, &data, &this->emitterCountBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"e count cbuffer creation failed", L"error", MB_OK);
	}

	DirectX::XMFLOAT4 randVec;
	randVec.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	randVec.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	randVec.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	randVec.w = (1.0f);

	desc.ByteWidth = sizeof(DirectX::XMFLOAT4);
	data.pSysMem = &randVec;

	hr = this->gDevice->CreateBuffer(&desc, &data, &this->randomVecBufer);
	if (FAILED(hr))
	{
		MessageBox(0, L"random cbuffer creation failed", L"error", MB_OK);
	}

	DirectX::XMFLOAT4 tempPlayerPos;
	data.pSysMem = &tempPlayerPos;
	hr = this->gDevice->CreateBuffer(&desc, &data, &this->playerPosBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"playerpos cbuffer creation failed", L"error", MB_OK);
	}

	
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = 1024 * sizeof(Emitterlocation);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(Emitterlocation);
	desc.Usage = D3D11_USAGE_DYNAMIC;

	Emitterlocation *tempE = new Emitterlocation[1024];
	for (size_t i = 0; i < 1024; i++)
	{
		tempE[i].particleType = 0;
		tempE[i].position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		tempE[i].randomVector = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	data.pSysMem = tempE;

	hr = this->gDevice->CreateBuffer(&desc, &data, &this->eLocations);
	if (FAILED(hr))
	{
		MessageBox(0, L"e location cbuffer creation failed", L"error", MB_OK);
	}
	delete[] tempE;

	D3D11_BUFFER_SRV esrv;
	esrv.FirstElement = 0;
	esrv.NumElements = 1024;

	D3D11_SHADER_RESOURCE_VIEW_DESC esrvDesc;
	ZeroMemory(&esrvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	esrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	esrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	esrvDesc.Buffer = esrv;

	hr = this->gDevice->CreateShaderResourceView(this->eLocations, &esrvDesc, &this->emitterSRV);
	if (FAILED(hr))
	{
		MessageBox(0, L"emitter srv failed", L"error", MB_OK);
	}

	desc.ByteWidth = 50 * sizeof(Particle);
	desc.StructureByteStride = sizeof(Particle);

	Particle *stompParticles = new Particle[50];
	for (size_t i = 0; i < 50; i++)
	{
		stompParticles->position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		stompParticles->velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		stompParticles->age = 0.0f;
		stompParticles->type = 1;
	}


	hr = this->gDevice->CreateBuffer(&desc, nullptr, &this->stompParticles);
	if (FAILED(hr))
	{
		MessageBox(0, L"stomp particle cbuffer creation failed", L"error", MB_OK);
	}
	
	delete[] stompParticles;

	D3D11_BUFFER_SRV ssrv;
	ssrv.FirstElement = 0;
	ssrv.NumElements = 50;

	D3D11_SHADER_RESOURCE_VIEW_DESC ssrvDesc;
	ZeroMemory(&ssrvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	ssrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	ssrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	ssrvDesc.Buffer = ssrv;

	hr = this->gDevice->CreateShaderResourceView(this->stompParticles, &ssrvDesc, &this->stompSRV);
	if (FAILED(hr))
	{
		MessageBox(0, L"stomp srv failed", L"error", MB_OK);
	}
	
}

void Renderer::createParticleShaders()
{
	ID3D11Resource *r = nullptr;
	DXCALL(CreateDDSTextureFromFile(gDevice, L"Particle.dds", &r, &particle_srv, 0, nullptr));

	D3D11_SAMPLER_DESC sdesc;
	ZeroMemory(&sdesc, sizeof(sdesc));
	sdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	//sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	DXCALL(gDevice->CreateSamplerState(&sdesc, &particle_sampler));

	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	DXCALL(gDevice->CreateBlendState(&BlendState, &particle_blend));

	ID3D10Blob *csBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"ComputeShader.hlsl",
		NULL,
		NULL,
		"main",
		"cs_5_0",
		0,
		0,
		&csBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"compute shader compile failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &this->computeShader);
	if (FAILED(hr))
	{
		MessageBox(0, L"compute Shader creation failed", L"error", MB_OK);
	}
	csBlob->Release();

	ID3D10Blob *icsBlob = nullptr;
	  hr = D3DCompileFromFile(
		L"Inserter.hlsl",
		NULL,
		NULL,
		"main",
		"cs_5_0",
		0,
		0,
		&icsBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"inserter compute shader compile failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateComputeShader(icsBlob->GetBufferPointer(), icsBlob->GetBufferSize(), nullptr, &this->inserter);
	if (FAILED(hr))
	{
		MessageBox(0, L"inserter compute Shader creation failed", L"error", MB_OK);
	}
	icsBlob->Release();

	ID3D10Blob *sicsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"StompInserter.hlsl",
		NULL,
		NULL,
		"main",
		"cs_5_0",
		0,
		0,
		&sicsBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"stomp inserter compute shader compile failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateComputeShader(sicsBlob->GetBufferPointer(), sicsBlob->GetBufferSize(), nullptr, &this->stompInserter);
	if (FAILED(hr))
	{
		MessageBox(0, L" stomp inserter compute Shader creation failed", L"error", MB_OK);
	}
	sicsBlob->Release();

	ID3DBlob* pvsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"pVertex.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		0,
		0,
		&pvsBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L"pvsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(pvsBlob->GetBufferPointer(), pvsBlob->GetBufferSize(), NULL, &this->pVertex);

	if (FAILED(hr))
	{
		MessageBox(0, L"particle vertex shader creation failed", L"error", MB_OK);
	}
	pvsBlob->Release();

	ID3DBlob* pgsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"pGeometry.hlsl",
		NULL,
		NULL,
		"main",
		"gs_5_0",
		0,
		0,
		&pgsBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"paricle geometry shader compile failed", L"error", MB_OK);
	}
	hr = this->gDevice->CreateGeometryShader(pgsBlob->GetBufferPointer(), pgsBlob->GetBufferSize(), nullptr, &this->pGeometry);
	if (FAILED(hr))
	{
		MessageBox(0, L"paricle Geometry shader creation failed", L"error", MB_OK);
	}

	pgsBlob->Release();

	ID3DBlob *ppsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"pPixel.hlsl",
		NULL,
		NULL,
		"main",
		"ps_5_0",
		0,
		0,
		&ppsBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"particle psBlob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreatePixelShader(ppsBlob->GetBufferPointer(), ppsBlob->GetBufferSize(), nullptr, &this->pPixel);
	if (FAILED(hr))
	{
		MessageBox(0, L"paricle pixel shader creation failed", L"error", MB_OK);
	}

	ppsBlob->Release();
}

void Renderer::updateParticles(float dt, Map *map)
{
	this->updateEmitters(map);
	this->updateDTimeBuffer(dt);
	this->totalTime += dt;
	if (this->totalTime - this->lastParticleInsert > 0.1f)
	{
		this->lastParticleInsert = this->totalTime;
		this->gDeviceContext->CSSetShader(this->inserter, nullptr, 0);
		this->gDeviceContext->CSSetConstantBuffers(0, 1, &this->ParticleCount);
		this->gDeviceContext->CSSetConstantBuffers(1, 1, &this->emitterCountBuffer);
		this->gDeviceContext->CSSetConstantBuffers(2, 1, &this->randomVecBufer);

		this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->UAVS[0], &UAVFLAG);
		this->gDeviceContext->CSSetShaderResources(0, 1, &this->emitterSRV);


		this->gDeviceContext->Dispatch(1, 1, 1);
		this->gDeviceContext->CopyStructureCount(this->ParticleCount, 0 ,this->UAVS[0]);

		this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->nullUAV, &UAVFLAG);
		this->gDeviceContext->CSSetUnorderedAccessViews(1, 1, &this->nullUAV, &UAVFLAG);

	}

	this->gDeviceContext->CSSetShader(this->computeShader, nullptr, 0);
	this->gDeviceContext->CSSetConstantBuffers(0, 1, &this->ParticleCount);
	this->gDeviceContext->CSSetConstantBuffers(1, 1, &this->deltaTimeBuffer);

	this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->UAVS[0], &UAVFLAG);
	this->gDeviceContext->CSSetUnorderedAccessViews(1, 1, &this->UAVS[1], &startParticleCount);

	this->gDeviceContext->Dispatch(512, 1, 1);
	this->swapBuffers();

	this->gDeviceContext->CopyStructureCount(this->ParticleCount, 0, this->UAVS[0]);
	this->gDeviceContext->CopyStructureCount(this->inderectArgumentBuffer, 0, this->UAVS[0]);

	this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->nullUAV, &UAVFLAG);
	this->gDeviceContext->CSSetUnorderedAccessViews(1, 1, &this->nullUAV, &UAVFLAG);

}

void Renderer::shrinkMap(Map * map)
{
	/*std::vector<XMFLOAT3> vertices;


	for (int i = 0; i < 128; i++)
	{

		XMFLOAT3 vert = {
			sin(2 * XM_PI * i / 128.f) * map->radius,
			0.01f,
			cos(2 * XM_PI * i / 128.f) * map->radius
		};

		XMFLOAT3 vert2 = {
			sin(2 * XM_PI * (i + 1) / 128.f) * map->radius,
			0.01f,
			cos(2 * XM_PI * (i + 1) / 128.f) * map->radius
		};
		vertices.push_back({ 0.f, 0.f, 0.f });
		vertices.push_back(vert);
		vertices.push_back(vert2);
	}

	D3D11_MAPPED_SUBRESOURCE data;
	this->gDeviceContext->Map(this->debug_map_quad, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &vertices[0], (UINT)(vertices.size() * 3 * sizeof(float)));
	this->gDeviceContext->Unmap(this->debug_map_quad, 0);*/

	D3D11_MAPPED_SUBRESOURCE data;
	this->gDeviceContext->Map(this->shrinkBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	this->toatlShrunkAmount = 10.0f - map->radius;
	memcpy(data.pData, &this->toatlShrunkAmount, sizeof(float));
	this->gDeviceContext->Unmap(this->shrinkBuffer, 0);
}

void Renderer::createLightBuffers()
{
	dirLight ligth;
	ligth.lightColor = DirectX::XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	ligth.lightDirection = DirectX::XMFLOAT4(0.0f, -13.0f, 0.0f, 1.0f);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(dirLight);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));

	data.pSysMem = &ligth;

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->dLightBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"light buffer creation failed!", L"error", MB_OK);
	}
	desc.ByteWidth = sizeof(UINT) * 4;
	UINT init[4];
	for (size_t i = 0; i < 4; i++)
	{
		init[i] = 0;
	}

	data.pSysMem = &init;
	hr = this->gDevice->CreateBuffer(&desc, &data, &this->pointLightCountBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"light count buffer creation failed!", L"error", MB_OK);
	}

	pointLight pointL[256];
	for (size_t i = 0; i < 256; i++)
	{
		pointL[i].lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		pointL[i].lightPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		pointL[i].range = 0.0f;
			
	}

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = 256 * sizeof(pointLight);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(pointLight);
	desc.Usage = D3D11_USAGE_DYNAMIC;

	data.pSysMem = pointL;

	hr = this->gDevice->CreateBuffer(&desc, &data, &this->pLightBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L" point light buffer creation failed!", L"error", MB_OK);
	}

	D3D11_BUFFER_SRV srv;
	srv.FirstElement = 0;
	srv.NumElements = 256;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer = srv;

	hr = this->gDevice->CreateShaderResourceView(this->pLightBuffer, &srvDesc, &this->pLightSRV);
	if (FAILED(hr))
	{
		MessageBox(0, L"point light srv failed", L"error", MB_OK);
	}
}

void Renderer::createCameraBuffer()
{
	XMFLOAT4 temp = XMFLOAT4(0.0f, 0.0f,0.0f, 0.0f);
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(DirectX::XMFLOAT4);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &temp;

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->cameraPosBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"Camera buffer cration failed", L"error", MB_OK);
	}


}

void Renderer::updateCameraPosBuffer(Camera * cam)
{
	D3D11_MAPPED_SUBRESOURCE data;
	ZeroMemory(&data, sizeof(D3D11_MAPPED_SUBRESOURCE));

	this->gDeviceContext->Map(this->cameraPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &cam->pos, sizeof(DirectX::XMFLOAT3));

	this->gDeviceContext->Unmap(this->cameraPosBuffer, 0);
}

void Renderer::updatePointLights(Map * map)
{
	this->pointLightCount = 0;
	pointLight temp[256];
	for (size_t i = 0; i < map->entitys.size(); i++)
	{
		if (dynamic_cast<ArcaneProjectileSpell*>(map->entitys[i]) != nullptr)
		{
			temp[pointLightCount].lightColor = dynamic_cast<ArcaneProjectileSpell*>(map->entitys[i])->light.lightColor;
			temp[pointLightCount].lightPos = dynamic_cast<ArcaneProjectileSpell*>(map->entitys[i])->light.lightPos;
			temp[pointLightCount].range = dynamic_cast<ArcaneProjectileSpell*>(map->entitys[i])->light.range;
			pointLightCount++;
		}
		if (dynamic_cast<FireProjectileSpell*>(map->entitys[i]) != nullptr)
		{
			temp[pointLightCount].lightColor = dynamic_cast<FireProjectileSpell*>(map->entitys[i])->light.lightColor;
			temp[pointLightCount].lightPos = dynamic_cast<FireProjectileSpell*>(map->entitys[i])->light.lightPos;
			temp[pointLightCount].range = dynamic_cast<FireProjectileSpell*>(map->entitys[i])->light.range;
			pointLightCount++;
		}
		if (dynamic_cast<EarthProjectileSpell*>(map->entitys[i]) != nullptr)
		{
			temp[pointLightCount].lightColor = dynamic_cast<EarthProjectileSpell*>(map->entitys[i])->light.lightColor;
			temp[pointLightCount].lightPos = dynamic_cast<EarthProjectileSpell*>(map->entitys[i])->light.lightPos;
			temp[pointLightCount].range = dynamic_cast<EarthProjectileSpell*>(map->entitys[i])->light.range;
			pointLightCount++;
		}
	}
	D3D11_MAPPED_SUBRESOURCE data;
	this->gDeviceContext->Map(this->pointLightCountBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &this->pointLightCount, sizeof(UINT));
	this->gDeviceContext->Unmap(this->pointLightCountBuffer, 0);

	if (this->pointLightCount > 0)
	{
		this->gDeviceContext->Map(this->pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy(data.pData, temp, sizeof(pointLight) * this->pointLightCount);
		this->gDeviceContext->Unmap(this->pLightBuffer, 0);
	}
}

void Renderer::createFullScreenQuad()
{
	struct TriangleVertex
	{
		float x, y, z;
		float u, v;
	};

	TriangleVertex triangleVertices[6] =
	{
		1.f, -1.f, 0.0f,	//v0 pos
		1.0f, 1.0f,

		-1.f, -1.f, 0.0f,	//v1
		0.0f, 1.0f,

		-1.f, 1.f, 0.0f, //v2
		0.0f,  0.0f,

		//t2
		-1.f, 1.f, 0.0f,	//v0 pos
		0.0f, 0.0f,

		1.f, 1.f, 0.0f,	//v1
		1.0f, 0.0f,

		1.f, -1.f, 0.0f, //v2
		1.0f, 1.0f
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	this->gDevice->CreateBuffer(&bufferDesc, &data, &this->quadVertexBuffer);

}

void Renderer::createScoreQuad()
{
	struct TriangleVertex
	{
		float x, y, z;
		float u, v;
	};

	TriangleVertex triangleVertices[6] =
	{
		.25f, .75f, 0.0f,		//OO
		1.0f, 1.0f,				//OX

		-.25f, .75f, 0.0f,		//OO
		0.0f, 1.0f,				//XO

		-.25f, 1.f, 0.0f,		//XO
		0.0f,  0.0f,			//OO

		//t2
		-.25f, 1.f, 0.0f,		//XO
		0.0f, 0.0f,				//OO

		.25f, 1.f, 0.0f,		//OX
		1.0f, 0.0f,				//OO

		.25f, .75f, 0.0f,		//OO
		1.0f, 1.0f				//OX
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	this->gDevice->CreateBuffer(&bufferDesc, &data, &this->roundVertexBuffer);

}

void Renderer::createcpMenuShaders()
{
	HRESULT hr;
	ID3DBlob* vsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"cpMenuVS.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L" cp vsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &this->cpMenuVs);

	if (FAILED(hr))
	{
		MessageBox(0, L" cp vertex shader creation failed", L"error", MB_OK);
	}

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = this->gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->cpQuadLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L"cp menu input desc creation failed", L"error", MB_OK);
	}

	vsBlob->Release();

	ID3DBlob *psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"CpMenuPS.hlsl",
		NULL,
		NULL,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L" cp psBlob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &this->cpmenuPS);
	if (FAILED(hr))
	{
		MessageBox(0, L" cp pixel shader creation failed", L"error", MB_OK);
	}

	psBlob->Release();

}

void Renderer::loadTexture()
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	ID3D11Resource *texture = nullptr;
	HRESULT hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/cpMenuTexture2.png ", &texture, &this->cpMenuTexture);
	if (FAILED(hr)){
		MessageBox(0, L"texture creation failed", L"error", MB_OK);}

	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/mainMenu.png ", &texture, &this->mainMenuTexture);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/cuMenuTexture.png ", &texture, &this->cuMenuTexture);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/endMenuTexture.png ", &texture, &this->endMenuTexture);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/cuArcane.png ", &texture, &this->r1CUTextures[0]);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/cuFire.png ", &texture, &this->r1CUTextures[1]);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/cuWind.png ", &texture, &this->r1CUTextures[2]);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/cuEarth.png ", &texture, &this->r1CUTextures[3]);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/cuWater.png ", &texture, &this->r1CUTextures[4]);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/scoreBoardTexture.png ", &texture, &this->scoreBoardTexture);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}

	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/mapTexture.png ", &texture, &this->mapTexture);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}
	hr = DirectX::CreateWICTextureFromFile(this->gDevice, this->gDeviceContext, L"../Resources/textures/lavaTexture.png ", &texture, &this->lavaTexture);
	if (FAILED(hr)) {
		MessageBox(0, L"texture creation failed", L"error", MB_OK);
	}

	texture->Release();

}

void Renderer::createHPBuffers()
{
	XMFLOAT4 hpBar[12];
	//bar 1
	//t1
	hpBar[0] = { 1.18f, 0.01f, -0.1f, 1.0f };
	hpBar[1] = { -0.29f, 0.01f, -0.1f,  1.0f };
	hpBar[2] = { -0.29f, 0.01f, 0.13f, 1.0f };

	//t2
	hpBar[3] = { -0.29f, 0.01f, 0.13f, 1.0f };
	hpBar[4] = { 1.18f, 0.01f, 0.13f, 1.0f };
	hpBar[5] = { 1.18f, 0.01f, -0.1f, 1.0f };

	//bar 2
	//t1
	hpBar[6] = { 1.18f, 0.0f, -0.1f, -1.0f };
	hpBar[7] = { -0.29f, 0.0f, -0.1f, -1.0f };
	hpBar[8] = { -0.29f, 0.0f, 0.13f, -1.0f };

	//t2
	hpBar[9] = { -0.29f, 0.0f, 0.13f, -1.0f };
	hpBar[10] = { 1.18f, 0.0f, 0.13f, -1.0f };
	hpBar[11] = { 1.18f, 0.0f, -0.1f, -1.0f };

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = sizeof(XMFLOAT4 ) * 12;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = hpBar;

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->HPVertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"hp vertex buffer creation failed", L"error", MB_OK);
	}

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(XMFLOAT4);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	XMFLOAT4 temp = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	data.pSysMem = &temp;

	hr = this->gDevice->CreateBuffer(&desc, &data, &this->HPBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"hp buffer creation failed", L"error", MB_OK);
	}

}

void Renderer::createHPShaders()
{

	HRESULT hr;
	ID3DBlob* vsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"HPVS.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L"cooldown vsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &this->HPVS);

	if (FAILED(hr))
	{
		MessageBox(0, L" cooldown vertex shader creation failed", L"error", MB_OK);
	}

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = this->gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->HPInputLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L"cooldwon input desc creation failed", L"error", MB_OK);
	}

	vsBlob->Release();

	ID3DBlob *psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"HPPS.hlsl",
		NULL,
		NULL,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"cooldown psBlob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &this->HPPS);
	if (FAILED(hr))
	{
		MessageBox(0, L" cooldown pixel shader creation failed", L"error", MB_OK);
	}

	psBlob->Release();
}

void Renderer::createCuBuffers()
{
	chooseUpgradesVertex v[24];
	v[0].pos = { 0.0f, 0.0f, 0.0f ,1.0f };
	v[1].pos = { -1.0f, 0.0f, 0.0f ,1.0f };
	v[2].pos = { -1.0f, 1.0f, 0.0f ,1.0f };
	v[3].pos = { -1.0f, 1.0f, 0.0f ,1.0f };
	v[4].pos = { 0.0f, 1.0f, 0.0f ,1.0f };
	v[5].pos = { 0.0f, 0.0f, 0.0f ,1.0f };

	v[6].pos = { 1.0f, 0.0f, 0.0f ,1.0f };
	v[7].pos = { 0.0f, 0.0f, 0.0f ,1.0f };
	v[8].pos = { 0.0f, 1.0f, 0.0f ,1.0f };
	v[9].pos = { 0.0f, 1.0f, 0.0f ,1.0f };
	v[10].pos = { 1.0f, 1.0f, 0.0f ,1.0f };
	v[11].pos = { 1.0f, 0.0f, 0.0f ,1.0f };

	v[12].pos = { 0.0f, -1.0f, 0.0f ,1.0f };
	v[13].pos = { -1.0f, -1.0f, 0.0f ,1.0f };
	v[14].pos = { -1.0f, 0.0f, 0.0f ,1.0f };
	v[15].pos = { -1.0f, 0.0f, 0.0f ,1.0f };
	v[16].pos = { 0.0f, 0.0f, 0.0f ,1.0f };
	v[17].pos = { 0.0f, -1.0f, 0.0f ,1.0f };

	v[18].pos = { 1.0f, -1.0f, 0.0f ,1.0f };
	v[19].pos = { 0.0f, -1.0f, 0.0f ,1.0f };
	v[20].pos = { 0.0f, 0.0f, 0.0f ,1.0f };
	v[21].pos = { 0.0f, 0.0f, 0.0f ,1.0f };
	v[22].pos = { 1.0f, 0.0f, 0.0f ,1.0f };
	v[23].pos = { 1.0f, -1.0f, 0.0f ,1.0f };


	v[0].uv = { 1.0f, 1.0f };
	v[1].uv = { 0.0f, 1.0f };
	v[2].uv = { 0.0f, 0.0f };
	v[3].uv = { 0.0f, 0.0f };
	v[4].uv = { 1.0f, 0.0f };
	v[5].uv = { 1.0f ,1.0f };

	v[6].uv = { 1.0f, 1.0f };
	v[7].uv = { 0.0f, 1.0f };
	v[8].uv = { 0.0f, 0.0f };
	v[9].uv = { 0.0f, 0.0f };
	v[10].uv = { 1.0f, 0.0f };
	v[11].uv = { 1.0f ,1.0f };

	v[12].uv = { 1.0f, 1.0f };
	v[13].uv = { 0.0f, 1.0f };
	v[14].uv = { 0.0f, 0.0f };
	v[15].uv = { 0.0f, 0.0f };
	v[16].uv = { 1.0f, 0.0f };
	v[17].uv = { 1.0f ,1.0f };

	v[18].uv = { 1.0f, 1.0f };
	v[19].uv = { 0.0f, 1.0f };
	v[20].uv = { 0.0f, 0.0f };
	v[21].uv = { 0.0f, 0.0f };
	v[22].uv = { 1.0f, 0.0f };
	v[23].uv = { 1.0f ,1.0f };


	v[0].index = 0;
	v[1].index = 0;
	v[2].index = 0;
	v[3].index = 0;
	v[4].index = 0;
	v[5].index = 0;

	v[6].index = 1;
	v[7].index = 1;
	v[8].index = 1;
	v[9].index = 1;
	v[10].index = 1;
	v[11].index = 1;

	v[12].index = 2;
	v[13].index = 2;
	v[14].index = 2;
	v[15].index = 2;
	v[16].index = 2;
	v[17].index = 2;

	v[18].index = 3;
	v[19].index = 3;
	v[20].index = 3;
	v[21].index = 3;
	v[22].index = 3;
	v[23].index = 3;


	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(v);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = v;
	HRESULT hr = this->gDevice->CreateBuffer(&bufferDesc, &data, &this->cuVertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"upgrades menu vertex buffer failed", L"error", MB_OK);
	}
}

void Renderer::createCUShaders()
{
	HRESULT hr;
	ID3DBlob* vsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"CUVS.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L"cooldown vsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &this->cuVS);

	if (FAILED(hr))
	{
		MessageBox(0, L" cooldown vertex shader creation failed", L"error", MB_OK);
	}

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INDEX", 0, DXGI_FORMAT_R32_SINT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = this->gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->cuLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L"cooldwon input desc creation failed", L"error", MB_OK);
	}

	vsBlob->Release();

	ID3DBlob *psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"CUPS.hlsl",
		NULL,
		NULL,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"cooldown psBlob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &this->cuPS);
	if (FAILED(hr))
	{
		MessageBox(0, L" cooldown pixel shader creation failed", L"error", MB_OK);
	}

	psBlob->Release();
}

void Renderer::createMapResurces()
{
	HRESULT hr;
	ID3DBlob* vsBlob = compile_shader(L"MapVS.hlsl", "main", "vs_5_0", gDevice);

	hr = this->gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &this->MapVS);

	if (FAILED(hr))
	{
		MessageBox(0, L" map vertex shader creation failed", L"error", MB_OK);
	}

	vsBlob = compile_shader(L"lavaVS.hlsl", "main", "vs_5_0", gDevice);

	hr = this->gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &this->lavaVS);

	if (FAILED(hr))
	{
		MessageBox(0, L" map vertex shader creation failed", L"error", MB_OK);
	}

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = this->gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->mapLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L"map input desc creation failed", L"error", MB_OK);
	}

	vsBlob->Release();

	ID3DBlob *psBlob = compile_shader(L"MapPS.hlsl", "main", "ps_5_0", gDevice);

	hr = this->gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &this->MapPS);
	if (FAILED(hr))
	{
		MessageBox(0, L" map pixel shader creation failed", L"error", MB_OK);
	}



	psBlob = compile_shader(L"lavaPS.hlsl", "main", "ps_5_0", gDevice);

	hr = this->gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &this->lavaPS);
	if (FAILED(hr))
	{
		MessageBox(0, L" lava pixel shader creation failed", L"error", MB_OK);
	}

	psBlob->Release();

	struct TriangleVertex
	{
		float x, y, z, w;
		float u, v;
	};

	TriangleVertex triangleVertices[6] =
	{
		10.f, 0.f, -10.f, 1.0f,	//v0 pos
		1.0f, 1.0f,

		-10.f, 0.f, -10.f, 1.0f,	//v1
		0.0f, 1.0f,

		-10.f, 0.f, 10.f, 1.0f, //v2
		0.0f,  0.0f,

		//t2
		-10.f, 0.f, 10.0f, 1.0f,//v0 pos
		0.0f, 0.0f,

		10.f, 0.f, 10.0f, 1.0f,//v1
		1.0f, 0.0f,

		10.f, 0.f, -10.0f, 1.0f,//v2
		1.0f, 1.0f
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	hr = this->gDevice->CreateBuffer(&bufferDesc, &data, &this->mapVBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L" map vBuffer creation failed", L"error", MB_OK);
	}

	TriangleVertex lavaTriangleVertices[6] =
	{
		22.5f, -0.19f, -22.5f, 1.0f,	//v0 pos
		1.0f, 1.0f,

		-22.5f, -0.19f, -22.5f, 1.0f,	//v1
		0.0f, 1.0f,

		-22.5f, -0.19f, 22.5f, 1.0f, //v2
		0.0f,  0.0f,

		//t2
		-22.5f, -0.19f, 22.5f, 1.0f,//v0 pos
		0.0f, 0.0f,

		22.5f, -0.19f, 22.5f, 1.0f,//v1
		1.0f, 0.0f,

		22.5f, -0.19f, -22.5f, 1.0f,//v2
		1.0f, 1.0f
	};

	data.pSysMem = lavaTriangleVertices;

	hr = this->gDevice->CreateBuffer(&bufferDesc, &data, &this->lavaBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"lava vBuffer creation failed", L"error", MB_OK);
	}
	
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth = sizeof(float) * 4;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	float init[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	data.pSysMem = init;

	hr = this->gDevice->CreateBuffer(&bufferDesc, &data, &this->shrinkBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L" map shrink buffer creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateBuffer(&bufferDesc, &data, &this->heatHazeBuffer);

	if (FAILED(hr))
	{
		MessageBox(0, L" heat haze buffer creation failed", L"error", MB_OK);
	}


	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	this->gDevice->CreateBlendState(&BlendState, &this->mapBlendState);


}

void Renderer::createCooldownBuffers()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(UINT) * 4;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	UINT init[4];
	for (size_t i = 0; i < 4; i++)
	{
		init[i]= 1;
	}


	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	
	data.pSysMem = &init;

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data, &this->cooldownBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"cooldown buffer creation failed", L"error", MB_OK);
	}

	std::vector<XMFLOAT4> vertices;

	for (size_t j = 0; j < 4; j++)
	{
		for (int i = 0; i < 128; i++)
		{

			XMFLOAT4 vert = {
				sin(2 * XM_PI * i / 128.f) * 0.1f + 0.3f * (float)j,
				0.01f,
				cos(2 * XM_PI * i / 128.f) * 0.1f,
				(float)j
			};

			XMFLOAT4 vert2 = {
				sin(2 * XM_PI * (i + 1) / 128.f) * 0.1f + 0.3f * (float)j,
				0.01f,
				cos(2 * XM_PI * (i + 1) / 128.f) * 0.1f,
				(float)j
			};
			vertices.push_back({ 0.3f * (float)j, 0.f, 0.f, (float)j });
			vertices.push_back(vert);
			vertices.push_back(vert2);

		}
	}
	


	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = (UINT)(vertices.size() * 4 * sizeof(float));
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;


	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &vertices[0];

	this->gDevice->CreateBuffer(&desc, &data, &this->cooldownCircles);
	if (FAILED(hr))
	{
		MessageBox(0, L"cooldown verex buffer creation failed", L"error", MB_OK);
	}
}

void Renderer::createCooldownShaders()
{
	HRESULT hr;
	ID3DBlob* vsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"cooldownVS.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob,
		nullptr);

	if (FAILED(hr))
	{
		MessageBox(0, L"cooldown vsblob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &this->cooldownVS);

	if (FAILED(hr))
	{
		MessageBox(0, L" cooldown vertex shader creation failed", L"error", MB_OK);
	}

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = this->gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &this->cooldownCirclesLayout);

	if (FAILED(hr))
	{
		MessageBox(0, L"cooldwon input desc creation failed", L"error", MB_OK);
	}

	vsBlob->Release();

	ID3DBlob *psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"cooldownPS.hlsl",
		NULL,
		NULL,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		NULL);
	if (FAILED(hr))
	{
		MessageBox(0, L"cooldown psBlob creation failed", L"error", MB_OK);
	}

	hr = this->gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &this->cooldownPS);
	if (FAILED(hr))
	{
		MessageBox(0, L" cooldown pixel shader creation failed", L"error", MB_OK);
	}

	psBlob->Release();
}

void Renderer::updatecooldownGUI(Player *player)
{
	UINT temp[4];
	for (size_t i = 0; i < 4; i++)
	{
		if (player->element->cooldown[i] < 0.001)
		{
			temp[i] = 1;
		}
		else
		{
			temp[i] = 0;
		}
	}
	
	D3D11_MAPPED_SUBRESOURCE data;
	this->gDeviceContext->Map(this->cooldownBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &temp, sizeof(UINT) * 4);
	this->gDeviceContext->Unmap(this->cooldownBuffer, 0);
}

void Renderer::updateHPBuffers(Player *player)
{
	float temp;

	temp = player->health / player->maxHealth;

	D3D11_MAPPED_SUBRESOURCE data;
	this->gDeviceContext->Map(this->HPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &temp, sizeof(float));
	this->gDeviceContext->Unmap(this->HPBuffer, 0);

}

void Renderer::updateheatHaze()
{
	if (gb)
	{
		this->heatHazeCounter += rand() + 0.42f;
	}
	else
	{
		this->heatHazeCounter -= rand() - 0.42f;
	}
	

	if (this->heatHazeCounter > 20000000.0f)
	{
		this->gb = false;
		//this->heatHazeCounter = 0.0f;
	}
	if (this->heatHazeCounter < -20000000.0f)
	{
		this->gb = true;
	}


	//this->heatHazeCounter += 0.1f;

	D3D11_MAPPED_SUBRESOURCE data;
	this->gDeviceContext->Map(this->heatHazeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &this->heatHazeCounter, sizeof(float));
	this->gDeviceContext->Unmap(this->heatHazeBuffer, 0);

}

void Renderer::renderShadowMap(Map * map, Camera * camera)
{
	float clear[] = { 0.f, 0.f, 0.f, 1.0f };
	gDeviceContext->ClearRenderTargetView(shadowMapRTV, clear);
	gDeviceContext->ClearDepthStencilView(DepthBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);

	gDeviceContext->VSSetShader(shadowMapVS, nullptr, 0);
	gDeviceContext->PSSetShader(nullptr, nullptr, 0);
	gDeviceContext->OMSetRenderTargets(0, nullptr, DepthBuffer);
	gDeviceContext->OMSetDepthStencilState(DepthStateReadWrite, 0x00);
	gDeviceContext->RSSetState(ShadowRaster);
	setViewPort(2048, 2048);
	gDeviceContext->VSSetConstantBuffers(0, 1, &shadow_wvp_buffer);

	shadow_camera.proj = XMMatrixOrthographicLH(50.f, 50.f, shadowznear, shadowzfar);
	XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&directionalLightPos), XMLoadFloat3(&directionalLightFocus), { 0, 1, 0 });
	shadow_camera.view = view;

	D3D11_MAPPED_SUBRESOURCE ddata;
	DXCALL(gDeviceContext->Map(shadow_wvp_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ddata));
	{
		CopyMemory(ddata.pData, &shadow_camera, sizeof(Camera::BufferVals));
	}
	gDeviceContext->Unmap(shadow_wvp_buffer, 0);

	gDeviceContext->IASetInputLayout(debug_entity_layout);


	mapmesh->PreDraw(gDevice, gDeviceContext);
	mapmesh->PrepareShaders();
	gDeviceContext->VSSetShader(shadowMapVS, nullptr, 0);

	gDeviceContext->PSSetShader(nullptr, nullptr, 0);
	mapmesh->Draw(gDevice, gDeviceContext);
	
	for (auto entity : map->entitys)
	{

		XMMATRIX &model = XMMatrixRotationAxis({ 0, 1, 0 }, XM_PI * 0.5f - entity->angle) * XMMatrixScaling(entity->radius, entity->radius, entity->radius) * XMMatrixTranslation(entity->position.x, entity->position.y + entity->radius, entity->position.z);

		//model = XMMatrixMultiply(XMMatrixRotationX(90 * XM_PI / 180), model);
		//model = XMMatrixMultiply(XMMatrixRotationZ(270 * XM_PI / 180), model);
		model = XMMatrixMultiply(XMMatrixRotationX(270 * XM_PI / 180), model);
		model = XMMatrixMultiply(XMMatrixRotationZ(90 * XM_PI / 180), model);
		model = XMMatrixMultiply(XMMatrixScaling(0.75f, 0.75f, 0.75f), model);
	
		shadow_camera.world = model;

		
		D3D11_MAPPED_SUBRESOURCE data;
		DXCALL(gDeviceContext->Map(shadow_wvp_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
		{
			CopyMemory(data.pData, &shadow_camera, sizeof(Camera::BufferVals));
		}
		gDeviceContext->Unmap(shadow_wvp_buffer, 0);
		
		if (entity->pMesh)
		{
			entity->pMesh->PreDraw(globalDevice, globalDeviceContext);
			gDeviceContext->PSSetShader(nullptr, nullptr, 0);
			gDeviceContext->VSSetShader(shadowMapVS, nullptr, 0);

			entity->pMesh->Draw(globalDevice, globalDeviceContext);
		}
			
	}
	
	shadow_camera.world = XMMatrixIdentity();

	D3D11_MAPPED_SUBRESOURCE data;
	DXCALL(gDeviceContext->Map(shadow_wvp_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
	{
		CopyMemory(data.pData, &shadow_camera, sizeof(Camera::BufferVals));
	}
	gDeviceContext->Unmap(shadow_wvp_buffer, 0);

	gDeviceContext->RSSetState(DefaultRaster);
	setViewPort(WIDTH, HEIGHT);
}

void Renderer::renderBlurPass(Map * map, Camera * cam)
{
	UINT32 stride = sizeof(float) * 4;
	UINT32 offset = 0u;

	gDeviceContext->IASetInputLayout(blur_fs_layout);
	gDeviceContext->IASetVertexBuffers(0, 1, &blur_fs_vertices, &stride, &offset);
	gDeviceContext->VSSetShader(blur_fs_vs, nullptr, 0);

	gDeviceContext->PSSetSamplers(0, 1, &blur_fs_sampler);

	// todo: remove, gammalt
	/*gDeviceContext->OMSetRenderTargets(1, &blur_rtv[1], nullptr);
	gDeviceContext->PSSetShader(gaussian_x_ps, nullptr, 0);
	gDeviceContext->PSSetShaderResources(0, 1, &blur_srv[0]);
	gDeviceContext->Draw(6, 0);

	gDeviceContext->PSSetShaderResources(0, 1, RESET_SRV);
	gDeviceContext->OMSetRenderTargets(1, RESET_RTV, nullptr);

	gDeviceContext->OMSetRenderTargets(1, &blur_rtv[0], nullptr);
	gDeviceContext->PSSetShader(gaussian_y_ps, nullptr, 0);
	gDeviceContext->PSSetShaderResources(0, 1, &blur_srv[1]);
	gDeviceContext->Draw(6, 0);*/

	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, nullptr);
	gDeviceContext->PSSetShader(blur_composite, nullptr, 0);
	gDeviceContext->PSSetShaderResources(0, 1, &default_srv);
	gDeviceContext->PSSetShaderResources(1, 1, &blur_srv[0]);
	gDeviceContext->Draw(6, 0);
}

void Renderer::renderCooldownGUI(Map * map, Camera * cam)
{
	this->gDeviceContext->IASetInputLayout(this->cooldownCirclesLayout);
	UINT32 size = sizeof(float) * 4;
	UINT32 offset = 0u;

	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->cooldownCircles, &size, &offset);
	this->gDeviceContext->VSSetShader(this->cooldownVS, nullptr, 0);
	this->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->cooldownPS, nullptr, 0);
	gDeviceContext->OMSetDepthStencilState(DepthStateDisable, 0xff);
	

	for (auto entity : map->entitys)
	{
		if (dynamic_cast<Player*>(entity) != nullptr)
		{


			XMMATRIX model = XMMatrixTranslation(entity->position.x - 0.4f, 2.5f, entity->position.z + 1.4f);

			cam->vals.world = model;
			cam->update(0, gDeviceContext);

			gDeviceContext->VSSetConstantBuffers(0, 1, &cam->wvp_buffer);
			this->updatecooldownGUI(dynamic_cast<Player*>(entity));
			this->gDeviceContext->PSSetConstantBuffers(0, 1, &this->cooldownBuffer);
			gDeviceContext->Draw(4 * 128 * 3, 0);
		}
	}


}

void Renderer::renderHPGUI(Map * map, Camera * cam)
{
	this->gDeviceContext->IASetInputLayout(this->HPInputLayout);
	UINT32 size = sizeof(float) * 4;
	UINT32 offset = 0u;

	this->gDeviceContext->IASetVertexBuffers(0, 1, &this->HPVertexBuffer, &size, &offset);
	this->gDeviceContext->VSSetShader(this->HPVS, nullptr, 0);
	this->gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->HPPS, nullptr, 0);
	gDeviceContext->OMSetDepthStencilState(DepthStateDisable, 0xff);


	for (auto entity : map->entitys)
	{

		Player* p = dynamic_cast<Player*>(entity);
		if (p != nullptr)
		{

			if ((p->maxHealth - p->health) > 0.001f)
			{
				XMMATRIX model = XMMatrixTranslation(entity->position.x - 0.4f, 2.5f, entity->position.z + 1.69f);

				cam->vals.world = model;
				cam->update(0, gDeviceContext);

				gDeviceContext->VSSetConstantBuffers(0, 1, &cam->wvp_buffer);
				this->updateHPBuffers(p);
				this->gDeviceContext->VSSetConstantBuffers(1, 1, &this->HPBuffer);
				gDeviceContext->Draw(12, 0);
			}
			
		}
	}

	cam->vals.world = XMMatrixIdentity();
	cam->update(0, gDeviceContext);
}

void Renderer::swapBuffers()
{
	ID3D11UnorderedAccessView *tempUAV;
	ID3D11ShaderResourceView *tempSRV;
	tempUAV = this->UAVS[0];
	tempSRV = this->SRVS[0];

	this->UAVS[0] = this->UAVS[1];
	this->SRVS[0] = this->SRVS[1];

	this->UAVS[1] = tempUAV;
	this->SRVS[1] = tempSRV;

	tempSRV = nullptr;
	tempUAV = nullptr;
}

void Renderer::renderParticles(Camera *camera)
{
	
	this->gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	this->gDeviceContext->VSSetShader(this->pVertex, nullptr, 0);
	this->gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	this->gDeviceContext->GSSetShader(this->pGeometry, nullptr, 0);
	this->gDeviceContext->PSSetShader(this->pPixel, nullptr, 0);
	this->gDeviceContext->PSSetShaderResources(0, 1, &particle_srv);
	this->gDeviceContext->PSSetSamplers(0, 1, &particle_sampler);
	this->gDeviceContext->IASetInputLayout(nullptr);
	this->gDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

	this->gDeviceContext->VSSetShaderResources(0, 0, nullptr);
	this->gDeviceContext->VSSetShaderResources(0, 1, &this->SRVS[0]);
	this->gDeviceContext->GSSetConstantBuffers(0, 1, &camera->floatwvpBuffer);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	this->gDeviceContext->OMSetBlendState(particle_blend, blendFactor, sampleMask);
	this->gDeviceContext->OMSetRenderTargets(1, &default_rtv, DepthBufferMS);

	this->gDeviceContext->DrawInstancedIndirect(this->inderectArgumentBuffer, 0);

	this->gDeviceContext->VSSetShaderResources(0, 1, &this->nullSRV);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);

}

void Renderer::renderMap(Camera * cam)
{
	gDeviceContext->IASetInputLayout(this->mapLayout);

	UINT32 size = sizeof(float) * 6;
	UINT32 offset = 0u;
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	gDeviceContext->OMSetBlendState(this->mapBlendState, blendFactor, sampleMask);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetVertexBuffers(0, 1, &this->lavaBuffer, &size, &offset);

	gDeviceContext->VSSetConstantBuffers(0, 1, &cam->wvp_buffer);
	gDeviceContext->PSSetConstantBuffers(0, 1, &cam->wvp_buffer);
	gDeviceContext->PSSetConstantBuffers(1, 1, &color_buffer);
	gDeviceContext->PSSetConstantBuffers(2, 1, &this->dLightBuffer);
	gDeviceContext->PSSetConstantBuffers(3, 1, &this->cameraPosBuffer);
	gDeviceContext->PSSetConstantBuffers(4, 1, &this->pointLightCountBuffer);
	gDeviceContext->PSSetConstantBuffers(5, 1, &this->shadow_wvp_buffer);
	gDeviceContext->PSSetConstantBuffers(6, 1, &this->heatHazeBuffer);
	gDeviceContext->PSSetConstantBuffers(7, 1, &this->deltaTimeBuffer);
	gDeviceContext->PSSetShaderResources(0, 1, &this->pLightSRV);
	gDeviceContext->PSSetShaderResources(1, 1, &this->DepthBufferSRV);
	gDeviceContext->PSSetShaderResources(2, 1, &this->lavaTexture);
	gDeviceContext->PSSetSamplers(0, 1, &this->shadowMapSampler);
	gDeviceContext->VSSetShader(this->lavaVS, nullptr, 0);
	gDeviceContext->PSSetShader(this->lavaPS, nullptr, 0);

	gDeviceContext->Draw(6, 0);

	gDeviceContext->IASetVertexBuffers(0, 1, &this->mapVBuffer, &size, &offset);
	gDeviceContext->VSSetShader(this->MapVS, nullptr, 0);
	gDeviceContext->VSSetConstantBuffers(1, 1, &this->shrinkBuffer);
	gDeviceContext->PSSetShader(this->MapPS, nullptr, 0);
	gDeviceContext->PSSetShaderResources(2, 1, &this->mapTexture);
	gDeviceContext->Draw(6, 0);
}

void Renderer::updateDTimeBuffer(float dt)
{
	D3D11_MAPPED_SUBRESOURCE data;
	this->gDeviceContext->Map(this->deltaTimeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &dt, sizeof(dt));
	this->gDeviceContext->Unmap(this->deltaTimeBuffer, 0);
}

void Renderer::updateEmitters(Map * map)
{
	this->emitterCount = 0;
	Emitterlocation *temp = new Emitterlocation[1024];
	for (size_t i = 0; i < map->entitys.size(); i++)
	{
		if (dynamic_cast<WindBeaconSpell*>(map->entitys[i]) != nullptr && dynamic_cast<WindBeaconSpell*>(map->entitys[i])->hasStomped())
		{
			this->createStompParticles(dynamic_cast<WindBeaconSpell*>(map->entitys[i])->position, 1);
			dynamic_cast<WindBeaconSpell*>(map->entitys[i])->endStomped();
		}
		if (dynamic_cast<Player*>(map->entitys[i]) != nullptr)
		{
			if (dynamic_cast<Player*>(map->entitys[i])->stomped)
			{
				this->createStompParticles(dynamic_cast<Player*>(map->entitys[i])->position, 1);
			}
			if (dynamic_cast<Player*>(map->entitys[i])->blowUp)
			{
				Player *temp = dynamic_cast<Player*>(map->entitys[i]);
				this->createStompParticles(dynamic_cast<FireElement*>(temp->element)->active_projectile->position, 1);
				temp->blowUp = false;
				dynamic_cast<FireElement*>(temp->element)->active_projectile = nullptr;
			}
		}
		if (dynamic_cast<ArcaneProjectileSpell*>(map->entitys[i]) != nullptr)
		{	
			ArcaneProjectileSpell* test = dynamic_cast<ArcaneProjectileSpell*>(map->entitys[i]);
			temp[emitterCount].position = test->pEmitter.position;
			temp[emitterCount].randomVector = test->pEmitter.randomVector;
			temp[emitterCount].particleType = test->pEmitter.particleType;
			emitterCount++;
		}
		if (dynamic_cast<WaterProjectileSpell*>(map->entitys[i]) != nullptr && dynamic_cast<WaterProjectileSpell*>(map->entitys[i])->pEmitter.particleType != -1)
		{
			WaterProjectileSpell* test = dynamic_cast<WaterProjectileSpell*>(map->entitys[i]);
			temp[emitterCount].position = test->pEmitter.position;
			temp[emitterCount].randomVector = test->pEmitter.randomVector;
			temp[emitterCount].particleType = test->pEmitter.particleType;
			emitterCount++;
		}
		if (map->entitys[i]->type == EntityType::emitter)
		{
			this->createStompParticles(map->entitys[i]->position, 1);
			map->entitys[i]->dead = true;
		}
		if (dynamic_cast<FirePathSpell* >(map->entitys[i]) != nullptr)
		{
			FirePathSpell* test = dynamic_cast<FirePathSpell*>(map->entitys[i]);
			temp[emitterCount].position = test->pEmitter.position;
			temp[emitterCount].randomVector = test->pEmitter.randomVector;
			temp[emitterCount].particleType = test->pEmitter.particleType;
			emitterCount++;
		}
	}
	
	D3D11_MAPPED_SUBRESOURCE data;

	this->gDeviceContext->Map(this->emitterCountBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &this->emitterCount, sizeof(this->emitterCount));
	this->gDeviceContext->Unmap(this->emitterCountBuffer, 0);

	if (this->emitterCount > 0)
	{
		this->gDeviceContext->Map(this->eLocations, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy(data.pData, temp, sizeof(Emitterlocation)*this->emitterCount);
		this->gDeviceContext->Unmap(this->eLocations, 0);
	}

	this->gDeviceContext->Map(this->randomVecBufer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	DirectX::XMFLOAT4 randVec;
	randVec.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	randVec.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	randVec.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	randVec.w = (1.0f);
	memcpy(data.pData, &randVec, sizeof(DirectX::XMFLOAT4));
	this->gDeviceContext->Unmap(this->randomVecBufer, 0);
	delete[] temp;
}

void Renderer::createStompParticles(DirectX::XMFLOAT3 pos, int type)
{
	D3D11_MAPPED_SUBRESOURCE data;
	this->gDeviceContext->Map(this->playerPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &pos, sizeof(DirectX::XMFLOAT3));
	this->gDeviceContext->Unmap(this->playerPosBuffer, 0);

	float lenght = 0.0f;
	Particle *particles = new Particle[50];
	for (size_t i = 0; i < 50; i++)
	{
		particles[i].age = 1.6f;
		particles[i].type = type;
		particles[i].velocity.y = 0.5f;

		//creates circle of particles around player
		particles[i].velocity.x = (0.55f) * cos( 7.2f *i * XM_PI / 180);
		particles[i].velocity.z = (0.55f) * sin(7.2f *i * XM_PI / 180);
		lenght = sqrt(particles[i].velocity.x * particles[i].velocity.x + particles[i].velocity.y * particles[i].velocity.y + particles[i].velocity.z * particles[i].velocity.z);

		particles[i].velocity.x = particles[i].velocity.x / lenght;
		
		particles[i].velocity.y = particles[i].velocity.y / lenght;
		
		particles[i].velocity.z = particles[i].velocity.z / lenght;

		particles[i].velocity.x *= 9.0f;
		particles[i].velocity.z *= 9.0f;

		particles[i].position = pos;
		particles[i].position.y = 0.2f;


	}
	gDeviceContext->Map(this->stompParticles, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, particles, sizeof(Particle) * 50);
	gDeviceContext->Unmap(this->stompParticles, 0);
	delete[] particles;


	this->gDeviceContext->CSSetShader(this->stompInserter, nullptr, 0);
	this->gDeviceContext->CSSetConstantBuffers(0, 1, &this->playerPosBuffer);
	//this->gDeviceContext->CSSetConstantBuffers(1, 1, &this->stompParticles);
	this->gDeviceContext->CSSetConstantBuffers(1, 1, &this->ParticleCount);
	this->gDeviceContext->CSSetShaderResources(0, 1, &this->stompSRV);

	this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->UAVS[0], &UAVFLAG);

	this->gDeviceContext->Dispatch(1, 1, 1);
	this->gDeviceContext->CopyStructureCount(this->ParticleCount, 0, this->UAVS[0]);

	this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->nullUAV, &startParticleCount);
	this->gDeviceContext->CSSetShaderResources(0, 1, &this->nullSRV);

}

void Renderer::render(Map *map, Camera *camera)
{
	dirLight light;
	light.lightColor = { 0.9f, 0.9f, 0.9f, 1.0f };
	light.lightDirection = { directionalLightPos.x, directionalLightPos.y, directionalLightPos.z, 1 };
	D3D11_MAPPED_SUBRESOURCE dddata;
	DXCALL(gDeviceContext->Map(dLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dddata));
	{
		CopyMemory(dddata.pData, &light, sizeof(dirLight));
	}
	gDeviceContext->Unmap(dLightBuffer, 0);


	renderShadowMap(map, camera);

	this->updateCameraPosBuffer(camera);
	XMFLOAT4 clear = normalize_color(0x93a9bcff);

	gDeviceContext->ClearRenderTargetView(default_rtv, (float*)&clear);
	gDeviceContext->ClearDepthStencilView(DepthBufferMS, D3D11_CLEAR_DEPTH, 1.0f, 0);
	gDeviceContext->OMSetDepthStencilState(DepthStateReadWrite, 0xff);
	gDeviceContext->OMSetRenderTargets(1, &default_rtv, DepthBufferMS);

	{
		XMFLOAT4 col = normalize_color(0x343434AA);
		D3D11_MAPPED_SUBRESOURCE data;
		DXCALL(gDeviceContext->Map(color_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
		{
			CopyMemory(data.pData, &col, sizeof(float) * 4);
		}
		gDeviceContext->Unmap(color_buffer, 0);


		UINT32 size = sizeof(float) * 6;
		UINT32 offset = 0u;
		gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		gDeviceContext->VSSetConstantBuffers(0, 1, &camera->wvp_buffer);
		gDeviceContext->PSSetConstantBuffers(0, 1, &camera->wvp_buffer);
		gDeviceContext->PSSetConstantBuffers(1, 1, &color_buffer);
		gDeviceContext->PSSetConstantBuffers(2, 1, &this->dLightBuffer);
		gDeviceContext->PSSetConstantBuffers(3, 1, &this->cameraPosBuffer);
		gDeviceContext->PSSetConstantBuffers(4, 1, &this->pointLightCountBuffer);
		gDeviceContext->PSSetConstantBuffers(5, 1, &this->shadow_wvp_buffer);

		gDeviceContext->PSSetShaderResources(0, 1, &this->pLightSRV);
		gDeviceContext->PSSetShaderResources(1, 1, &this->DepthBufferSRV);
		gDeviceContext->PSSetSamplers(0, 1, &this->shadowMapSampler);

	}
	{
		XMFLOAT4 col = normalize_color(0x998D66ff);
		D3D11_MAPPED_SUBRESOURCE data;
		DXCALL(gDeviceContext->Map(color_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
		{
			CopyMemory(data.pData, &col, sizeof(float) * 4);
		}
		gDeviceContext->Unmap(color_buffer, 0);
	}
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	gDeviceContext->OMSetBlendState(NULL, blendFactor, sampleMask);
	mapmesh->PreDraw(gDevice, gDeviceContext);
	mapmesh->PrepareShaders();
	mapmesh->Draw(gDevice, gDeviceContext);





	gDeviceContext->OMSetDepthStencilState(DepthStateReadWrite, 0xff);

	{
		gDeviceContext->IASetInputLayout(debug_entity_layout);

		UINT32 size = sizeof(float) * 3;
		UINT32 offset = 0u;
		gDeviceContext->IASetVertexBuffers(0, 1, &debug_entity_circle, &size, &offset);
		gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		//sets stuff twice fix if bad prefomance
		gDeviceContext->VSSetShader(debug_entity_vsh, nullptr, 0);
		gDeviceContext->PSSetShader(debug_entity_psh, nullptr, 0);
		gDeviceContext->PSSetConstantBuffers(0, 1, &camera->wvp_buffer);
		gDeviceContext->PSSetConstantBuffers(1, 1, &color_buffer);
		gDeviceContext->PSSetConstantBuffers(2, 1, &this->dLightBuffer);
		gDeviceContext->PSSetConstantBuffers(3, 1, &this->cameraPosBuffer);
		gDeviceContext->PSSetConstantBuffers(4, 1, &this->pointLightCountBuffer);

		gDeviceContext->PSSetShaderResources(0, 1, &this->pLightSRV);

		int i = 0;
		int colors[4] = {
			0x9EB6D3ff,
			0xD19C9Cff,
			0x9ACE9Fff,
			0xBA99CCff
		};
		for (auto entity : map->entitys)
		{
			gDeviceContext->IASetVertexBuffers(0, 1, &debug_entity_circle, &size, &offset);
			gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
			gDeviceContext->IASetInputLayout(debug_entity_layout);
			gDeviceContext->VSSetShader(debug_entity_vsh, nullptr, 0);
			gDeviceContext->PSSetShader(debug_entity_psh, nullptr, 0);

			XMFLOAT4 col = normalize_color(i >= 4 ? (0xfff6b2ff * (++i)) : colors[i++]);
			D3D11_MAPPED_SUBRESOURCE data;
			DXCALL(gDeviceContext->Map(color_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
			{
				CopyMemory(data.pData, &col, sizeof(float) * 4);
			}
			gDeviceContext->Unmap(color_buffer, 0);

			XMMATRIX model = XMMatrixRotationAxis({ 0, 1, 0 }, XM_PI * 0.5f - entity->angle) * XMMatrixScaling(entity->radius, entity->radius, entity->radius) * XMMatrixTranslation(entity->position.x, entity->position.y, entity->position.z);

			camera->vals.world = model;
			camera->update(0, gDeviceContext);

			gDeviceContext->VSSetConstantBuffers(0, 1, &camera->wvp_buffer);
			gDeviceContext->Draw(129, 0);

			if (entity->pMesh != nullptr)
			{

				entity->pMesh->PreDraw(globalDevice, globalDeviceContext);
				model = XMMatrixMultiply(XMMatrixRotationX(270 * XM_PI / 180), model);
				model = XMMatrixMultiply(XMMatrixRotationZ(90 * XM_PI / 180), model);

				model = XMMatrixMultiply(XMMatrixScaling(0.75f, 0.75f, 0.75f), model);

				camera->vals.world = model;
				camera->update(0, gDeviceContext);
				gDeviceContext->PSSetConstantBuffers(0, 1, &camera->wvp_buffer);
				gDeviceContext->PSSetConstantBuffers(1, 1, &color_buffer);
				gDeviceContext->PSSetConstantBuffers(2, 1, &this->dLightBuffer);
				gDeviceContext->PSSetConstantBuffers(3, 1, &this->cameraPosBuffer);
				gDeviceContext->PSSetConstantBuffers(4, 1, &this->pointLightCountBuffer);
				gDeviceContext->PSSetShaderResources(0, 1, &this->pLightSRV);


				entity->pMesh->Draw(globalDevice, globalDeviceContext);
			}
			if (entity->pAnimator != nullptr)
			{
				float scale = entity->pAnimator->_mesh->scale;
				XMMATRIX model = XMMatrixRotationAxis({ 0, 1, 0 }, XM_PI * 0.5f - entity->angle) * XMMatrixScaling(scale, scale, scale) * XMMatrixTranslation(entity->position.x, entity->position.y + entity->radius, entity->position.z);

				model = XMMatrixMultiply(XMMatrixRotationX(270 * XM_PI / 180), model);
				model = XMMatrixMultiply(XMMatrixRotationZ(90 * XM_PI / 180), model);

				camera->vals.world = model;
				camera->update(0, gDeviceContext);
				gDeviceContext->PSSetConstantBuffers(0, 1, &camera->wvp_buffer);
				gDeviceContext->PSSetConstantBuffers(1, 1, &color_buffer);
				gDeviceContext->PSSetConstantBuffers(2, 1, &this->dLightBuffer);
				gDeviceContext->PSSetConstantBuffers(3, 1, &this->cameraPosBuffer);
				gDeviceContext->PSSetConstantBuffers(4, 1, &this->pointLightCountBuffer);
				gDeviceContext->PSSetShaderResources(0, 1, &this->pLightSRV);

				//TODO: deltaTime
				entity->pAnimator->DrawAndUpdate(0.01f);

			}
		}

	}

	ID3D11ShaderResourceView *reset = nullptr;
	gDeviceContext->PSSetShaderResources(1, 1, &reset);

	camera->vals.world = XMMatrixIdentity();
	camera->update(0, gDeviceContext);
	this->renderMap(camera);
	this->renderCooldownGUI(map, camera);
	this->renderHPGUI(map, camera);

	this->renderParticles(camera);
	
	FXSystem->render(camera, default_rtv, default_srv, blur_rtv[0], blur_rtv[1]);
	gDeviceContext->GenerateMips(blur_srv[0]);
	// TODO: behövs antagligen inte mer
	std::swap(default_rtv, blur_rtv[1]);
	std::swap(default_srv, blur_srv[1]);
	renderBlurPass(map, camera);
}


void Renderer::present() {
	this->gSwapChain->Present(0, 0);
}

void Renderer::update(float dt, Map *map, Camera *camera)
{
	FXSystem->update(camera, dt);
	this->updateParticles(dt, map);
	this->updatePointLights(map);
	this->updateheatHaze();
	if (map->shrunk == true)
	{
		map->shrunk = false;
		this->shrinkMap(map);
	}
}
