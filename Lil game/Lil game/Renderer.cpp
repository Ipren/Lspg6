#include "Renderer.h"

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

	this->height = height;
	this->width = width;

	this->createDirect3DContext(wndHandle);
	this->createDepthBuffers();
	this->createShaders();
	this->setViewPort(width, height);
	this->create_debug_entity();

}

Renderer::~Renderer()
{
	this->gBackbufferRTV->Release();
	this->gDepthStencil->Release();
	this->gDevice->Release();
	this->gDeviceContext->Release();
	this->gSwapChain->Release();
	this->debug_map_layout->Release();
	this->debug_map_quad->Release();
	this->debug_map_psh->Release();
	this->debug_map_vsh->Release();
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
	float vertices[] = {
		-10, 0, -10,
		-10, 0,  10,
		10, 0,  10,

		-10, 0, -10,
		10, 0,  10,
		10, 0, -10
	};

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(vertices);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &vertices;

	DXCALL(gDevice->CreateBuffer(&desc, &data, &debug_map_quad));

	ID3DBlob *blob = compile_shader(L"Simple.hlsl", "VS", "vs_5_0", this->gDevice);
	DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &debug_map_vsh));

	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	debug_map_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, this->gDevice);

	blob = compile_shader(L"Simple.hlsl", "PS", "ps_5_0", this->gDevice);
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &debug_map_psh));
	blob->Release();
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

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	DXCALL(gDevice->CreateDepthStencilView(pDepthStencil, &descDSV, &gDepthStencil));

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

void Renderer::render(Map *map, Camera *camera)
{

	float clear[] = { 0, 0, 0, 1 };

	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, clear);
	gDeviceContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	{
		gDeviceContext->IASetInputLayout(debug_map_layout);

		UINT32 size = sizeof(float) * 3;
		UINT32 offset = 0u;
		gDeviceContext->IASetVertexBuffers(0, 1, &debug_map_quad, &size, &offset);
		gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		gDeviceContext->VSSetShader(debug_map_vsh, nullptr, 0);
		gDeviceContext->VSSetConstantBuffers(0, 1, &camera->wvp_buffer);
		gDeviceContext->PSSetShader(debug_map_psh, nullptr, 0);

		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthStencil);

		gDeviceContext->Draw(6, 0);
	}

	
	{
		gDeviceContext->IASetInputLayout(debug_entity_layout);

		UINT32 size = sizeof(float) * 3;
		UINT32 offset = 0u;
		gDeviceContext->IASetVertexBuffers(0, 1, &debug_entity_circle, &size, &offset);
		gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		gDeviceContext->VSSetShader(debug_entity_vsh, nullptr, 0);
		gDeviceContext->PSSetShader(debug_entity_psh, nullptr, 0);

		for (auto entity : map->entitys)
		{
			XMMATRIX model = XMMatrixRotationAxis({ 0, 1, 0 }, entity->angle) * XMMatrixScaling(entity->radious, 1, entity->radious) * XMMatrixTranslation(entity->position.x, 0, entity->position.z);

			camera->vals.world = model;
			camera->update(0, gDeviceContext);

			gDeviceContext->VSSetConstantBuffers(0, 1, &camera->wvp_buffer);
			gDeviceContext->Draw(129, 0);
		}
	}

	this->gSwapChain->Present(0,0);
}
