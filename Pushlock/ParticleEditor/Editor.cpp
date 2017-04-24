#include "Editor.h"

#include <d3d11.h>
#include <d3dcompiler.h>

#include <vector>
#include <algorithm>

#include "External/DirectXTK.h"
#include "External/imgui.h"
#include "External/imgui_dock.h"

#include "External/Helpers.h"
#include "External/dxerr.h"

#include "Globals.h"
#include "Camera.h"
#include "Ease.h"

#include "Particle.h"

using namespace DirectX;

Camera *camera;
float time;
float ptime;

std::vector<ParticleDefinition> definitions;
std::vector<Particle> particles;

std::vector<ParticleEffect> effects;
ParticleEffect *current_effect;

Editor::Settings default_settings;
Editor::Settings settings;

D3D11_VIEWPORT viewport;
bool viewport_render;
bool viewport_dirty = true;

// Plane
ID3D11Buffer *plane_vertex_buffer;
ID3D11InputLayout *plane_layout;

ID3D11VertexShader *plane_vs;
ID3D11PixelShader *plane_ps;
ID3D11ShaderResourceView *plane_srv;
ID3D11SamplerState *plane_sampler;

ID3D11RenderTargetView *default_rtv;
ID3D11ShaderResourceView *default_srv;

ID3D11BlendState *no_blend;

// Particle
ID3D11Buffer *particle_buffer;
ID3D11InputLayout *particle_layout;

ID3D11VertexShader *particle_vs;
ID3D11GeometryShader *particle_gs;
ID3D11PixelShader *particle_ps;
ID3D11ShaderResourceView *particle_srv;
ID3D11SamplerState *particle_sampler;

ID3D11BlendState *particle_blend;

// Composite
ID3D11Buffer *composite_vertex_buffer;
ID3D11InputLayout *composite_layout;

ID3D11VertexShader *composite_vs;
ID3D11PixelShader *composite_ps;
ID3D11SamplerState *composite_sampler;

ID3D11RenderTargetView *hdr_rtv;
ID3D11ShaderResourceView *hdr_srv;

float RandomFloat(float lo, float hi)
{
	return ((hi - lo) * ((float)rand() / RAND_MAX)) + lo;
}

void ComboFunc(const char *label, ParticleEase *ease)
{
	ImGui::Combo(label, (int*)ease, EASE_STRINGS);
}

void SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)WIDTH;
	vp.Height = (float)HEIGHT;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vp);
}

void InitPlane()
{
	float vertices[] = {
		-25.f, -0.1f, -25.f,
		-25.f, -0.1f,  25.f,
		25.f, -0.1f,  25.f,

		-25.f, -0.1f, -25.f,
		25.f, -0.1f,  25.f,
		25.f, -0.1f, -25.f
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

	DXCALL(gDevice->CreateBuffer(&desc, &data, &plane_vertex_buffer));

	ID3DBlob *blob = compile_shader(L"Resources/Plane.hlsl", "VS", "vs_5_0", gDevice);
	DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &plane_vs));

	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	plane_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, gDevice);

	blob = compile_shader(L"Resources/Plane.hlsl", "PS", "ps_5_0", gDevice);
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &plane_ps));

	ID3D11Resource *r = nullptr;
	DXCALL(CreateDDSTextureFromFile(gDevice, L"Resources/Plane.dds", &r, &plane_srv, 0, nullptr));

	D3D11_SAMPLER_DESC sampdesc;
	ZeroMemory(&sampdesc, sizeof(sampdesc));
	sampdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampdesc.Filter = D3D11_FILTER_ANISOTROPIC;
	DXCALL(gDevice->CreateSamplerState(&sampdesc, &plane_sampler));

	ID3D11Texture2D *tex;
	D3D11_TEXTURE2D_DESC rtv_desc;
	rtv_desc.Width = WIDTH;
	rtv_desc.Height = HEIGHT;
	rtv_desc.Usage = D3D11_USAGE_DEFAULT;
	rtv_desc.MipLevels = 1;
	rtv_desc.ArraySize = 1;
	rtv_desc.SampleDesc.Count = 1;
	rtv_desc.SampleDesc.Quality = 0;
	rtv_desc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
	rtv_desc.CPUAccessFlags = 0;
	rtv_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rtv_desc.MiscFlags = 0;

	DXCALL(gDevice->CreateTexture2D(&rtv_desc, nullptr, &tex));

	D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
	ZeroMemory(&sdesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	D3D11_RENDER_TARGET_VIEW_DESC rdesc;
	ZeroMemory(&sdesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	sdesc.Texture2D.MipLevels = 1;
	sdesc.Texture2D.MostDetailedMip = 0;

	rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rdesc.Texture2D.MipSlice = 0;

	DXCALL(gDevice->CreateShaderResourceView(tex, &sdesc, &default_srv));
	DXCALL(gDevice->CreateRenderTargetView(tex, &rdesc, &default_rtv));
}

void InitParticles()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = (UINT)(sizeof(Particle) * 4096);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	DXCALL(gDevice->CreateBuffer(&desc, nullptr, &particle_buffer));

	ID3DBlob *blob = compile_shader(L"Resources/Particle.hlsl", "VS", "vs_5_0", gDevice);
	DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &particle_vs));

	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "ORIGIN", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 80, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATION", 0, DXGI_FORMAT_R32_FLOAT, 0, 88, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATIONV", 0, DXGI_FORMAT_R32_FLOAT, 0, 92, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "FOG", 0, DXGI_FORMAT_R32_FLOAT, 0, 96, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TYPE", 0, DXGI_FORMAT_R32_SINT, 0, 100, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "IDX", 0, DXGI_FORMAT_R32_SINT, 0, 104, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	particle_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, gDevice);

	blob = compile_shader(L"Resources/Particle.hlsl", "GS", "gs_5_0", gDevice);
	DXCALL(gDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &particle_gs));

	blob = compile_shader(L"Resources/Particle.hlsl", "PS", "ps_5_0", gDevice);
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &particle_ps));

	ID3D11Resource *r = nullptr;
	DXCALL(CreateDDSTextureFromFile(gDevice, L"Resources/Particle.dds", &r, &particle_srv, 0, nullptr));

	D3D11_SAMPLER_DESC sadesc;
	ZeroMemory(&sadesc, sizeof(sadesc));
	sadesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sadesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sadesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sadesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	DXCALL(gDevice->CreateSamplerState(&sadesc, &particle_sampler));

	D3D11_BLEND_DESC state;
	ZeroMemory(&state, sizeof(D3D11_BLEND_DESC));
	state.RenderTarget[0].BlendEnable = TRUE;
	state.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	state.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	state.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	state.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	state.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	state.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	state.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	DXCALL(gDevice->CreateBlendState(&state, &particle_blend));

	state.RenderTarget[0].BlendEnable = FALSE;
	DXCALL(gDevice->CreateBlendState(&state, &no_blend));
}

void InitComposite()
{
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

	DXCALL(gDevice->CreateBuffer(&desc, &data, &composite_vertex_buffer));

	ID3DBlob *blob = compile_shader(L"Resources/Composite.hlsl", "VS", "vs_5_0", gDevice);
	DXCALL(gDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &composite_vs));

	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	composite_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, gDevice);

	blob = compile_shader(L"Resources/Composite.hlsl", "PS", "ps_5_0", gDevice);
	DXCALL(gDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &composite_ps));

	D3D11_SAMPLER_DESC sampdesc;
	ZeroMemory(&sampdesc, sizeof(sampdesc));
	sampdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	DXCALL(gDevice->CreateSamplerState(&sampdesc, &composite_sampler));

	ID3D11Texture2D *tex;
	D3D11_TEXTURE2D_DESC rtv_desc;
	rtv_desc.Width = WIDTH;
	rtv_desc.Height = HEIGHT;
	rtv_desc.Usage = D3D11_USAGE_DEFAULT;
	rtv_desc.MipLevels = 1;
	rtv_desc.ArraySize = 1;
	rtv_desc.SampleDesc.Count = 1;
	rtv_desc.SampleDesc.Quality = 0;
	rtv_desc.Format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
	rtv_desc.CPUAccessFlags = 0;
	rtv_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rtv_desc.MiscFlags = 0;

	DXCALL(gDevice->CreateTexture2D(&rtv_desc, nullptr, &tex));

	D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
	ZeroMemory(&sdesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	D3D11_RENDER_TARGET_VIEW_DESC rdesc;
	ZeroMemory(&sdesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	sdesc.Texture2D.MipLevels = 1;
	sdesc.Texture2D.MostDetailedMip = 0;

	rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	rdesc.Texture2D.MipSlice = 0;

	DXCALL(gDevice->CreateShaderResourceView(tex, &sdesc, &hdr_srv));
	DXCALL(gDevice->CreateRenderTargetView(tex, &rdesc, &hdr_rtv));
}

void RenderPlane()
{
	UINT32 stride = sizeof(float) * 3;
	UINT32 offset = 0u;

	gDeviceContext->IASetInputLayout(plane_layout);
	gDeviceContext->IASetVertexBuffers(0, 1, &plane_vertex_buffer, &stride, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gDeviceContext->VSSetShader(plane_vs, nullptr, 0);
	gDeviceContext->VSSetConstantBuffers(0, 1, &camera->wvp_buffer);

	gDeviceContext->PSSetShader(plane_ps, nullptr, 0);
	gDeviceContext->PSSetSamplers(0, 1, &plane_sampler);
	gDeviceContext->PSSetShaderResources(0, 1, &plane_srv);

	gDeviceContext->OMSetRenderTargets(1, &hdr_rtv, gDepthbufferDSV);

	gDeviceContext->Draw(6, 0);

	ID3D11RenderTargetView *rtv = nullptr;
	gDeviceContext->OMSetRenderTargets(1, &rtv, nullptr);
}

void RenderParticles()
{
	UINT32 stride = sizeof(Particle);
	UINT32 offset = 0u;

	gDeviceContext->IASetInputLayout(particle_layout);
	gDeviceContext->IASetVertexBuffers(0, 1, &particle_buffer, &stride, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	gDeviceContext->VSSetShader(particle_vs, nullptr, 0);
	
	gDeviceContext->GSSetShader(particle_gs, nullptr, 0);
	gDeviceContext->GSSetConstantBuffers(0, 1, &camera->wvp_buffer);

	gDeviceContext->PSSetShader(particle_ps, nullptr, 0);
	gDeviceContext->PSSetConstantBuffers(0, 1, &camera->wvp_buffer);
	gDeviceContext->PSSetSamplers(0, 1, &particle_sampler);
	gDeviceContext->PSSetShaderResources(0, 1, &particle_srv);
	gDeviceContext->PSSetShaderResources(1, 1, &gDepthbufferSRV);

	float factor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT mask = 0xffffffff;
	
	gDeviceContext->OMSetBlendState(particle_blend, factor, mask);
	gDeviceContext->OMSetDepthStencilState(gDepthRead, 0xff);
	gDeviceContext->OMSetRenderTargets(1, &hdr_rtv, gDepthbufferDSV);

	gDeviceContext->Draw(particles.size(), 0);

	ID3D11RenderTargetView *rtv = nullptr;
	ID3D11ShaderResourceView *srv = nullptr;
	gDeviceContext->PSSetShaderResources(1, 1, &srv);
	gDeviceContext->OMSetRenderTargets(1, &rtv, nullptr);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->OMSetDepthStencilState(gDepthReadWrite, 0xff);

	
}

void RenderComposite()
{
	UINT32 stride = sizeof(float) * 4;
	UINT32 offset = 0u;

	gDeviceContext->IASetInputLayout(composite_layout);
	gDeviceContext->IASetVertexBuffers(0, 1, &composite_vertex_buffer, &stride, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	gDeviceContext->VSSetShader(composite_vs, nullptr, 0);

	gDeviceContext->PSSetShader(composite_ps, nullptr, 0);
	gDeviceContext->PSSetSamplers(0, 1, &composite_sampler);
	gDeviceContext->PSSetShaderResources(0, 1, &default_srv);
	gDeviceContext->PSSetShaderResources(1, 1, &hdr_srv);

	float factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	gDeviceContext->OMSetBlendState(no_blend, factor, 0xffffffff);
	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, nullptr);

	gDeviceContext->Draw(6, 0);

	ID3D11ShaderResourceView *srvs[] = { nullptr, nullptr };
	gDeviceContext->PSSetShaderResources(0, 2, srvs);
}

namespace Editor {

static bool menu_open = false;
static int current_fx = 0;
static int current_def = 0;

void Style()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.19f, 0.95f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.19f, 0.95f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.41f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.11f, 0.59f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.48f, 0.78f, 0.32f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.59f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.48f, 0.80f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.62f, 0.62f, 0.62f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.95f, 0.92f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.95f, 0.92f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.67f, 0.40f, 0.40f, 0.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.11f, 0.59f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.47f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.28f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.48f, 0.80f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	style.AntiAliasedLines = false;

	style.WindowRounding = 0.f;
	style.ScrollbarRounding = 0.f;
	style.ScrollbarSize = 15;
}

void MenuBar()
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("open")) {
			OPENFILENAME ofn;
			wchar_t szFileName[MAX_PATH] = L"";

			ZeroMemory(&ofn, sizeof(ofn));

			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = wndHandle;
			ofn.lpstrFilter = L"No Files (*.no)\0*.no\0All Files (*.*)\0*.*\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrDefExt = L"no";

			if (GetOpenFileName(&ofn)) {

				effects.clear();
				definitions.clear();

				DeserializeParticles(ofn.lpstrFile, effects, definitions);
			}
		}
		if (ImGui::MenuItem("save", nullptr, nullptr, !definitions.empty() || !effects.empty())) {
			OPENFILENAME ofn;
			wchar_t szFileName[MAX_PATH] = L"";

			ZeroMemory(&ofn, sizeof(ofn));

			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = wndHandle;
			ofn.lpstrFilter = L"No Files (*.no)\0*.no\0All Files (*.*)\0*.*\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
			ofn.lpstrDefExt = L"no";

			if (GetSaveFileName(&ofn)) {
				SerializeParticles(ofn.lpstrFile, effects, definitions);
			}
		}
		if (ImGui::MenuItem("save editor layout")) {
			ImGui::SaveDock();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Settings")) {
		ImGui::TextDisabled("Camera");
		ImGui::SliderFloat("distance", &settings.CameraDistance, 0.1f, 8.f);
		ImGui::SliderFloat("height", &settings.CameraHeight, 0.0f, 8.f);
		ImGui::SliderFloat("speed", &settings.CameraSpeed, -1.5f, 1.5f);
		ImGui::TextDisabled("Simulation");
		ImGui::SliderFloat("speed##part", &settings.ParticleSpeed, -1.5f, 1.5f);
		ImGui::Checkbox("paused", &settings.ParticlePaused);

		if (ImGui::Button("reset")) {
			settings = default_settings;
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
}

void ParticleEditor()
{
	if (ImGui::BeginDock("Particles")) {
		ImGui::BeginGroup();
		ImGui::BeginChild("list", ImVec2(120, -ImGui::GetItemsLineHeightWithSpacing()), true);
		for (int i = 0; i < definitions.size(); i++)
		{
			char label[64];
			sprintf_s(label, 64, "%s##%d", definitions[i].name, i);
			if (ImGui::Selectable(label, current_def == i)) {
				current_def = i;
			}
		}
		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));
		if (!definitions.empty()) {
			ParticleDefinition *def = &definitions[current_def];

			ImGui::Text("%s", def->name);
			ImGui::Separator();
			ImGui::InputText("name", def->name, 32);
			ImGui::Combo("type", (int*)&def->orientation, "Planar\0Clip\0Velocity\0VelocityAnchored\0");


			ImGui::InputFloat("gravity", &def->gravity);
			ImGui::InputFloat("lifetime", &def->lifetime);

			ImGui::TextDisabled("Scale");
			ComboFunc("ease##scale", &def->scale_fn);
			ImGui::DragFloat("start##scale", &def->scale_start, 0.001f);
			ImGui::DragFloat("end##scale", &def->scale_end, 0.001f);

			ImGui::TextDisabled("Color");
			ComboFunc("ease##color", &def->color_fn);
			ImGui::ColorEdit4("start##color", (float*)&def->start_color);
			ImGui::ColorEdit4("end##color", (float*)&def->end_color);

			ImGui::TextDisabled("Texture");
			ImGui::InputInt4("uv", &def->u);

			float limit = ImGui::CalcItemWidth();

			auto size = (1 > (def->u2 / def->v2)) ?
				ImVec2(def->u2 * (limit / (float)def->v2), limit) :
				ImVec2(limit, def->v2 * (limit / (float)def->u2));

			ImGui::Image((void*)particle_srv, size, ImVec2(def->u / 2048.f, def->v / 2048.f), ImVec2((def->u + def->u2) / 2048.f, (def->v + def->v2) / 2048.f), ImVec4(def->start_color.x, def->start_color.y, def->start_color.z, def->start_color.w));
		}
		ImGui::EndChild();

		ImGui::BeginChild("buttons");
		if (ImGui::Button("Add")) {
			definitions.push_back({});
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove")) {

		}
		ImGui::EndChild();
		ImGui::EndGroup();
	}
	ImGui::EndDock();

}

void FXEditor()
{
	if (ImGui::BeginDock("FX")) {
		ImGui::BeginGroup();
		ImGui::BeginChild("list", ImVec2(120, -ImGui::GetItemsLineHeightWithSpacing()), true);
		for (int i = 0; i < effects.size(); i++)
		{
			char label[64];
			sprintf_s(label, 64, "%s##%d", effects[i].name, i);
			if (ImGui::Selectable(label, current_fx == i)) {

				current_fx = i;
				effects[current_fx].age = 0.f;

			}
		}
		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));
		if (!effects.empty()) {
			ParticleEffect *fx = &effects[current_fx];
			current_effect = fx;

			ImGui::Text("%s", fx->name);
			ImGui::Separator();
			ImGui::InputText("name##fx", fx->name, 32);

			ImGui::TextDisabled("Time");
			if (ImGui::Checkbox("loop", &fx->loop)) {
				fx->clamp_children = false;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("clamp to children", &fx->clamp_children)) {
				fx->loop = false;
			}

			if (fx->clamp_children) {
				for (int i = 0; i < current_effect->fx_count; ++i) {
					ParticleEffectEntry entry = fx->fx[i];
					if (entry.idx < 0) continue;

					auto def = definitions[entry.idx];

					fx->children_time = max(fx->children_time, entry.end);
				}
			}

			if (!fx->loop && !fx->clamp_children) {
				ImGui::DragFloat("duration", &fx->time, 0.025f, 0.f, 100000.f, "%.3f seconds");
			}

			ImGui::Separator();

			for (int i = 0; i < current_effect->fx_count; ++i) {
				ParticleEffectEntry *entry = &fx->fx[i];
				if (entry->idx < 0) continue;

				auto def = definitions[entry->idx];

				char label[64];
				sprintf_s(label, 64, "%s##%d", def.name, i);

				bool header = ImGui::TreeNode(label);
				ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - 50);
				ImGui::PushID(i);
				if (ImGui::SmallButton("Remove")) {
					for (int j = i; j < current_effect->fx_count - 1; j++) {
						fx->fx[i] = fx->fx[j + 1];
					}
					fx->fx[current_effect->fx_count - 1] = {};
					fx->fx_count--;
				}
				ImGui::PopID();

				if (header) {
					ImGui::TextDisabled("Time");
					ImGui::Checkbox("loop##psoda", &entry->loop);

					if (entry->loop) {
						ImGui::DragFloat("start", &entry->start, 0.01, 0.f, 10000.f, "%.3f sec");
					}
					else {
						ImGui::DragFloat("start", &entry->start, 0.01, 0.f, 10000.f, "%.3f sec");
						ImGui::DragFloat("duration", &entry->end, 0.01, 0.f, 10000.f, "%.3f sec");
					}

					ImGui::TextDisabled("Emitter");
					ImGui::Combo("type##emitter", (int*)&entry->emitter_type, EMITTER_STRINGS);
					switch (entry->emitter_type) {
					case ParticleEmitter::Cube:
						ImGui::TextDisabled("Spawn area");
						ImGui::DragFloatRange2("x", &entry->emitter_xmin, &entry->emitter_xmax, 0.01f);
						ImGui::DragFloatRange2("y", &entry->emitter_ymin, &entry->emitter_ymax, 0.01f);
						ImGui::DragFloatRange2("z", &entry->emitter_zmin, &entry->emitter_zmax, 0.01f);

						ImGui::TextDisabled("Spawn velocity");
						ImGui::DragFloatRange2("x##spawn", &entry->vel_xmin, &entry->vel_xmax, 0.01f);
						ImGui::DragFloatRange2("y##spawn", &entry->vel_ymin, &entry->vel_ymax, 0.01f);
						ImGui::DragFloatRange2("z##spawn", &entry->vel_zmin, &entry->vel_zmax, 0.01f);
						break;
					case ParticleEmitter::Sphere:
						break;
					}

					ImGui::TextDisabled("Rotation");
					ImGui::DragFloatRange2("start##rotsp", &entry->rot_min, &entry->rot_max, 0.01f);
					ImGui::DragFloatRange2("velocity##rotps", &entry->rot_vmin, &entry->rot_vmax, 0.01f);

					if (entry->emitter_type != ParticleEmitter::Static) {
						ImGui::TextDisabled("Spawn rate");
						ComboFunc("ease##spawn", &entry->spawn_fn);

						ImGui::DragInt("start##sffffs", &entry->spawn_start, 1.0f, 0, 500, "%.0f particles/s");
						ImGui::DragInt("end##sdasd", &entry->spawn_end, 1.0f, 0, 500, "%.0f particles/s");
					}

					ImGui::TreePop();
				}
			}

			ImGui::Separator();

			static int add_pdef = 0;

			if (ImGui::Button("Add##entry")) {
				ImGui::OpenPopup("Add particle");
				add_pdef = 0;
			}

			if (ImGui::BeginPopupModal("Add particle")) {

				ImGui::BeginChild("list popiup", ImVec2(150, 200), true);
				for (int i = 0; i < definitions.size(); i++)
				{
					char label[64];
					sprintf_s(label, 64, "%s##asd%d", definitions[i].name, i);
					if (ImGui::Selectable(label, add_pdef == i)) {
						add_pdef = i;
					}
				}
				ImGui::EndChild();
				ImGui::SameLine();
				if (ImGui::Button("Add##pop")) {
					auto &entry = current_effect->fx[current_effect->fx_count++];
					entry.idx = add_pdef;
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel##popup")) {
					ImGui::CloseCurrentPopup();
				}



				ImGui::EndPopup();
			}
		}

		ImGui::EndChild();

		ImGui::BeginChild("buttons");
		if (ImGui::Button("Add##fx")) {
			ParticleEffect fx = {};
			effects.push_back(fx);
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove")) {

		}
		ImGui::EndChild();
		ImGui::EndGroup();
	}
	ImGui::EndDock();
}

void Timeline()
{
	if (ImGui::BeginDock("Timeline")) {

		ImGui::BeginGroup();

		ImGui::BeginChild("playstuff", ImVec2(150, 0), true);

		ImGui::Checkbox("paused##controls", &settings.ParticlePaused);
		ImGui::SameLine();
		ImGui::Checkbox("loop##controls", &settings.ParticleLoop);

		ImGui::SliderFloat("speed##part", &settings.ParticleSpeed, -1.5f, 1.5f);



		ImGui::EndChild();

		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		if (!effects.empty()) {
			auto fx = effects[current_fx];
			auto time = fx.clamp_children ? fx.children_time : fx.time;

			ImGui::ProgressBar(fx.age / time);
			ImGui::NewLine();

			auto hi = (int)ceilf(time);

			for (int i = 0; i < (int)hi; ++i) {
				if (i <= (int)time) {

					ImGui::SameLine(ImGui::GetContentRegionAvailWidth() * (i / (float)time));
					ImGui::TextDisabled("%.1f", (float)i);
				}
			}

			ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - 20);
			ImGui::TextDisabled("%.1f", time);
		}
		else {
			ImGui::ProgressBar(0.f);
		}

		ImDrawList* draw_list = ImGui::GetWindowDrawList();



		auto h = ImGui::GetContentRegionAvail().y;
		auto cur = ImGui::GetCursorScreenPos();

		ImGui::BeginChild("timelineview", ImVec2(0, 0), true);

		auto acur = ImGui::GetCursorScreenPos();
		auto w = ImGui::GetContentRegionAvailWidth();

		if (!effects.empty()) {
			auto fx = effects[current_fx];
			auto time = fx.clamp_children ? fx.children_time : fx.time;
			auto hi = (int)ceilf(time);

			for (int i = 0; i < (int)hi; ++i) {
				auto extend = 4.f;
				if (i <= (int)time) {
					draw_list->AddLine(
						ImVec2(acur.x + w * (i / (float)time), cur.y - extend),
						ImVec2(acur.x + w * (i / (float)time), cur.y + h + extend),
						ImColor(ImGui::GetStyle().Colors[ImGuiCol_Border]),
						2.f
					);
				}

				for (int j = 0; j < 4; ++j) {
					draw_list->AddLine(
						ImVec2(acur.x + w * (((float)i + j / 4.f) / (float)time), cur.y),
						ImVec2(acur.x + w * (((float)i + j / 4.f) / (float)time), cur.y + h),
						ImColor(ImGui::GetStyle().Colors[ImGuiCol_Border]),
						1.f
					);
				}
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 3));


			for (int i = 0; i < current_effect->fx_count; ++i) {
				ParticleEffectEntry entry = current_effect->fx[i];
				if (entry.idx < 0) continue;

				auto def = definitions[entry.idx];
				ImGui::SameLine(w * (entry.start / time));

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 1.0, 1.0, 0.23));
				if (entry.loop)
					ImGui::Button(def.name, ImVec2(w, 0));
				else
					ImGui::Button(def.name, ImVec2(w * (entry.end) / time, 0));
				ImGui::PopStyleColor(1);


				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 1.0, 1.0, 0.1));
				if (!entry.loop) {
					ImGui::SameLine();
					ImGui::Button("lifetime", ImVec2(w * (def.lifetime) / time, 0));
				}
				ImGui::PopStyleColor(1);

				ImGui::Separator();
				ImGui::NewLine();
			}

			ImGui::PopStyleVar(1);
		}
		ImGui::EndChild();
		ImGui::EndGroup();
	}
	ImGui::EndDock();
}

void Init()
{
	ImGui::LoadDock();

	camera = new Camera({0.f, 0.5f, 0.f}, {});

	Style();

	InitPlane();
	InitParticles();
	InitComposite();
}

void Update(float dt)
{
	time += dt * settings.CameraSpeed;
	ptime += dt * settings.ParticleSpeed;

	MenuBar();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::RootDock(ImVec2(0, 18), ImVec2(WIDTH, HEIGHT - 18));
	ParticleEditor();
	FXEditor();
	Timeline();

	viewport_render = false;

	if (ImGui::BeginDock("Viewport")) {
		viewport_render = true;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));

		ImGui::BeginChild("vp render", ImVec2(0, 0), true);

		auto cur = ImGui::GetCursorScreenPos();
		auto sz = ImGui::GetContentRegionAvail();

		if (cur.x != viewport.TopLeftX ||
			cur.y != viewport.TopLeftY ||
			sz.x != viewport.Width ||
			sz.y != viewport.Height)
		{
			viewport_dirty = true;

			viewport.Width = sz.x;
			viewport.Height = sz.y;
			viewport.MaxDepth = 1.0f;
			viewport.MinDepth = 0.0f;
			viewport.TopLeftX = cur.x;
			viewport.TopLeftY = cur.y;
		}

		if (viewport_dirty && !ImGui::IsMouseDragging()) {
			viewport_dirty = false;

			gDeviceContext->RSSetViewports(1, &viewport);
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
	ImGui::EndDock();
	ImGui::PopStyleColor();

	auto pdt = dt * settings.ParticleSpeed;

	auto fx = current_effect;

	if (!settings.ParticlePaused && settings.ParticleLoop && fx != nullptr) {
		auto time = fx->clamp_children ? fx->children_time : fx->time;

		if (fx->age >= time) {
			fx->age = 0;
		}
	}

	if (fx != nullptr && !settings.ParticlePaused) {
		auto time = fx->clamp_children ? fx->children_time : fx->time;

		fx->age += pdt;
		if (fx->age >= time) {
			
		}
		else {
			for (int i = 0; i < fx->fx_count; ++i) {
				auto &entry = fx->fx[i];

				auto def = definitions[entry.idx];

				if (fx->age > entry.start && fx->age < entry.start + entry.end) {
					auto factor = (fx->age - entry.start) / (entry.end);

					auto spawn_ease = GetEaseFunc(entry.spawn_fn);
					float spawn = spawn_ease((float)entry.spawn_start, (float)entry.spawn_end, factor) * pdt;

					entry.spawned_particles += spawn;
					
					for (; entry.spawned_particles >= 1.f; entry.spawned_particles -= 1.f) {
						XMVECTOR pos = {
							RandomFloat(entry.emitter_xmin, entry.emitter_xmax),
							RandomFloat(entry.emitter_ymin, entry.emitter_ymax),
							RandomFloat(entry.emitter_zmin, entry.emitter_zmax),
						};

						XMVECTOR vel = {
							RandomFloat(entry.vel_xmin, entry.vel_xmax),
							RandomFloat(entry.vel_ymin, entry.vel_ymax),
							RandomFloat(entry.vel_zmin, entry.vel_zmax),
						};

						float rot = RandomFloat(entry.rot_min, entry.rot_max);
						float rotvel = RandomFloat(entry.rot_vmin, entry.rot_vmax);

						Particle p = {};
						p.origin = pos;
						p.pos = pos;
						p.velocity = vel;
						p.idx = entry.idx;
						p.rotation = rot;
						p.rotation_velocity = rotvel;
						p.type = (int)def.orientation;
						p.scale = { 1.f, 1.f };
						particles.push_back(p);
					}
				}
			}
		}
	}

	auto it = particles.begin();
	while (it != particles.end())
	{
		auto p = it;

		ParticleDefinition *def = &definitions[p->idx];
		
		auto age = p->age / def->lifetime;
		if (!settings.ParticlePaused) {
			p->pos += p->velocity * pdt;
			p->velocity -= { 0.f, def->gravity * pdt, 0.f, 0.f };
			p->rotation += p->rotation_velocity * pdt;
			p->age += pdt;
		}

		if (def->orientation == ParticleOrientation::Velocity) {
			p->origin = p->pos;
		}

		auto scale_fn = GetEaseFunc(def->scale_fn);
		p->scale = {
			scale_fn(def->scale_start, def->scale_end, age) * (def->u2 / 2048.f),
			scale_fn(def->scale_start, def->scale_end, age) * (def->v2 / 2048.f)
		};

		p->uv = { def->u / 2048.f, def->v / 2048.f, (def->u + def->u2) / 2048.f, (def->v + def->v2) / 2048.f };

		auto color_fn = GetEaseFuncV(def->color_fn);
		p->color = color_fn(
			XMLoadFloat4(&def->start_color),
			XMLoadFloat4(&def->end_color),
			age
		);

		if (p->age > def->lifetime) {
			it = particles.erase(it);
		}
		else {
			it++;
		}
	}

	std::sort(particles.begin(), particles.end(), [](Particle &a, Particle &b) { return XMVectorGetZ(XMVector3Length(camera->pos - a.pos)) > XMVectorGetZ(XMVector3Length(camera->pos - b.pos)); });

	if (!particles.empty()) {
		D3D11_MAPPED_SUBRESOURCE data;
		DXCALL(gDeviceContext->Map(particle_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
		{
			CopyMemory(data.pData, particles.data(), sizeof(Particle) * particles.size());
		}
		gDeviceContext->Unmap(particle_buffer, 0);
	}

	camera->pos = { sin(time) * settings.CameraDistance, settings.CameraHeight, cos(time) * settings.CameraDistance };
	camera->update(dt, viewport.Width, viewport.Height);
}

void Render(float dt)
{
	XMFLOAT4 clear = normalize_color(0x93a9bcff);
	auto col = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
	col.w = 1.0f;
	XMFLOAT4 bclear = normalize_color(ImGui::GetColorU32(col));

	gDeviceContext->ClearRenderTargetView(default_rtv, (float*)&clear);
	gDeviceContext->ClearRenderTargetView(hdr_rtv, (float*)&clear);
	gDeviceContext->ClearRenderTargetView(gBackbufferRTV, (float*)&col);
	gDeviceContext->ClearDepthStencilView(gDepthbufferDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);


	auto vp = viewport;
	//vp.TopLeftX = 0;
	//vp.TopLeftY = 0;

	if (viewport_render) {
		SetViewport();

		RenderPlane();
		RenderParticles();
		gDeviceContext->RSSetViewports(1, &vp);

		RenderComposite();
	}

	ImGui::Render();


}

}