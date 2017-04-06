#include "Renderer.h"
#include "Spell.h"

const UINT startParticleCount = 0;
//makes it so the actual amount of particles in the buffers is used
const UINT UAVFLAG = -1;

Renderer::Renderer()
{

}

Renderer::Renderer(HWND wndHandle, int width, int height)
{
	this->gBackbufferRTV = nullptr;
	this->gDepthStencil = nullptr;
	this->gDevice = nullptr;
	this->gDeviceContext = nullptr;
	this->gSwapChain = nullptr;

	this->debug_map_layout = nullptr;
	this->debug_map_psh = nullptr;
	this->debug_map_quad = nullptr;
	this->debug_map_vsh = nullptr;

	this->debug_entity_circle = nullptr;
	this->debug_entity_layout = nullptr;
	this->debug_entity_psh = nullptr;
	this->debug_entity_vsh = nullptr;

	this->nullSRV = nullptr;
	this->nullUAV = nullptr;
	this->nullRTV = nullptr;

	this->height = height;
	this->width = width;
	this->totalTime = 0.0f;
	this->lastParticleInsert = 0.0f;
	this->emitterCount = 0;

	this->createDirect3DContext(wndHandle);
	this->createDepthBuffers();
	this->createShaders();
	this->setViewPort(width, height);
	this->createParticleBuffer(2048);
	HRESULT hr = this->gDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void **>(&debugDevice));
	if (FAILED(hr))
	{
		MessageBox(0, L"debug device failed", L"error", MB_OK);
	}


	this->create_debug_entity();

}

Renderer::~Renderer()
{
	ULONG test = 0;
	this->gDeviceContext->ClearState();
	this->gBackbufferRTV->Release();
	this->gDepthStencil->Release();
	this->gDevice->Release();
	this->gDeviceContext->Release();
	this->gSwapChain->Release();
	this->debug_map_layout->Release();
	this->debug_map_quad->Release();
	this->debug_map_psh->Release();
	this->debug_map_vsh->Release();
	this->color_buffer->Release();
	this->debug_entity_circle->Release();
	this->debug_entity_layout->Release();
	this->debug_entity_psh->Release();
	this->debug_entity_vsh->Release();
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

	/*this->debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);*/
	this->debugDevice->Release();
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
	start.z += 0.4;

	vertices[0] = start;
	vertices.push_back(start);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(XMFLOAT3) * vertices.size();
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
	desc.ByteWidth = vertices.size() * 3 * sizeof(float);
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
	ID3D11Texture2D* pDepthStencil = NULL;
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
	DXCALL(gDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil));

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = true;
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

	ID3D11DepthStencilState * pDSState;
	DXCALL(gDevice->CreateDepthStencilState(&dsDesc, &pDSState));
	this->gDeviceContext->OMSetDepthStencilState(pDSState, 1);
	pDSState->Release();

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	DXCALL(gDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &gDepthStencil));
	pDepthStencil->Release();

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
		particles[i].type = 0.0f;
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

	HRESULT hr = this->gDevice->CreateBuffer(&desc, &data,  &particleBuffer1);
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

	desc.ByteWidth = 100 * sizeof(Emitterlocation);
	Emitterlocation *tempE = new Emitterlocation[100];
	for (size_t i = 0; i < 100; i++)
	{
		tempE[i].particleType = 0;
		tempE[i].position = DirectX::XMFLOAT3(1.1f, 1.1f, 1.1f);
		tempE[i].randomVector = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	}
	
	data.pSysMem = &tempE;

	hr = this->gDevice->CreateBuffer(&desc, &data, &this->eLocations);
	if (FAILED(hr))
	{
		MessageBox(0, L"e location cbuffer creation failed", L"error", MB_OK);
	}
	delete[] tempE;

	desc.ByteWidth = 4 * sizeof(int);
	data.pSysMem = &this->emitterCount;

	hr = this->gDevice->CreateBuffer(&desc, &data, &this->emitterCountBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"e count cbuffer creation failed", L"error", MB_OK);
	}
}

void Renderer::createParticleShaders()
{

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

void Renderer::updateParticles(float dt)
{
	this->updateDTimeBuffer(dt);
	this->totalTime += dt;
	if (this->totalTime - this->lastParticleInsert > 1.0f)
	{
		this->lastParticleInsert = this->totalTime;
		this->gDeviceContext->CSSetShader(this->inserter, nullptr, 0);
		this->gDeviceContext->CSSetConstantBuffers(0, 1, &this->eLocations);
		this->gDeviceContext->CSSetConstantBuffers(1, 1, &this->ParticleCount);
		this->gDeviceContext->CSSetConstantBuffers(2, 1, &this->emitterCountBuffer);

		this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->UAVS[0], &UAVFLAG);

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

	this->gDeviceContext->Dispatch(2, 1, 1);
	this->swapBuffers();

	this->gDeviceContext->CopyStructureCount(this->ParticleCount, 0, this->UAVS[0]);
	this->gDeviceContext->CopyStructureCount(this->inderectArgumentBuffer, 0, this->UAVS[0]);

	this->gDeviceContext->CSSetUnorderedAccessViews(0, 1, &this->nullUAV, &UAVFLAG);
	this->gDeviceContext->CSSetUnorderedAccessViews(1, 1, &this->nullUAV, &UAVFLAG);

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
	this->gDeviceContext->IASetInputLayout(nullptr);
	this->gDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

	this->gDeviceContext->VSSetShaderResources(0, 0, nullptr);
	this->gDeviceContext->VSSetShaderResources(0, 1, &this->SRVS[0]);
	this->gDeviceContext->GSSetConstantBuffers(0, 1, &camera->floatwvpBuffer);

	this->gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencil);

	this->gDeviceContext->DrawInstancedIndirect(this->inderectArgumentBuffer, 0);

	this->gDeviceContext->VSSetShaderResources(0, 1, &this->nullSRV);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);

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
	Emitterlocation *temp = new Emitterlocation[100];
	for (size_t i = 0; i < map->entitys.size(); i++)
	{
		if (dynamic_cast<PushSpell*>(map->entitys[i]) != nullptr)
		{
			emitterCount++;
			temp[emitterCount] = dynamic_cast<PushSpell*>(map->entitys[i])->pEmitter;
		}
	}
	D3D11_MAPPED_SUBRESOURCE data;
	this->gDeviceContext->Map(this->emitterCountBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, &this->emitterCount, sizeof(this->emitterCount));
	this->gDeviceContext->Unmap(this->emitterCountBuffer, 0);

	this->gDeviceContext->Map(this->eLocations, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy(data.pData, temp, sizeof(Emitterlocation)*this->emitterCount);
	this->gDeviceContext->Unmap(this->eLocations, 0);



	delete[] temp;
}

void Renderer::render(Map *map, Camera *camera)
{
	XMFLOAT4 clear = normalize_color(0x93a9bcff);

	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, (float*)&clear);
	gDeviceContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	{
		XMFLOAT4 col = normalize_color(0x5e6172ff);
		D3D11_MAPPED_SUBRESOURCE data;
		DXCALL(gDeviceContext->Map(color_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
		{
			CopyMemory(data.pData, &col, sizeof(float) * 4);
		}
		gDeviceContext->Unmap(color_buffer, 0);


		gDeviceContext->IASetInputLayout(debug_map_layout);

		UINT32 size = sizeof(float) * 3;
		UINT32 offset = 0u;
		gDeviceContext->IASetVertexBuffers(0, 1, &debug_map_quad, &size, &offset);
		gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		gDeviceContext->VSSetShader(debug_map_vsh, nullptr, 0);
		gDeviceContext->VSSetConstantBuffers(0, 1, &camera->wvp_buffer);
		gDeviceContext->PSSetShader(debug_map_psh, nullptr, 0);
		gDeviceContext->PSSetConstantBuffers(1, 1, &color_buffer);

		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencil);

		gDeviceContext->Draw(128*3, 0);
	}

	
	{
		gDeviceContext->IASetInputLayout(debug_entity_layout);

		UINT32 size = sizeof(float) * 3;
		UINT32 offset = 0u;
		gDeviceContext->IASetVertexBuffers(0, 1, &debug_entity_circle, &size, &offset);
		gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		gDeviceContext->VSSetShader(debug_entity_vsh, nullptr, 0);
		gDeviceContext->PSSetShader(debug_entity_psh, nullptr, 0);
		gDeviceContext->PSSetConstantBuffers(1, 1, &color_buffer);

		int i = 2;
		for (auto entity : map->entitys)
		{
			XMFLOAT4 col = normalize_color(0xfff6b2ff * (++i));
			D3D11_MAPPED_SUBRESOURCE data;
			DXCALL(gDeviceContext->Map(color_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
			{
				CopyMemory(data.pData, &col, sizeof(float) * 4);
			}
			gDeviceContext->Unmap(color_buffer, 0);

			XMMATRIX model = XMMatrixRotationAxis({ 0, 1, 0 }, XM_PI * 0.5 - entity->angle) * XMMatrixScaling(entity->radius, 1, entity->radius) * XMMatrixTranslation(entity->position.x, 0, entity->position.z);

			camera->vals.world = model;
			camera->update(0, gDeviceContext);

			gDeviceContext->VSSetConstantBuffers(0, 1, &camera->wvp_buffer);
			gDeviceContext->Draw(129, 0);
		}
	}

	this->updateEmitters(map);
	this->renderParticles(camera);
	this->gSwapChain->Present(0,0);
}
