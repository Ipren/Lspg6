#include "Particle.h"

#include <d3d11.h>
#include <algorithm>

#include "External\dxerr.h"
#include "External\Helpers.h"
#include "External\DirectXTK.h"

ID3D11RenderTargetView *RESET_RTV[16] = {};
ID3D11ShaderResourceView *RESET_SRV[16] = {};

inline float RandomFloat(float lo, float hi)
{
	return ((hi - lo) * ((float)rand() / RAND_MAX)) + lo;
}

ParticleSystem::ParticleSystem(UINT capacity)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = (UINT)(sizeof(Particle) * 4096);
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	DXCALL(device->CreateBuffer(&desc, nullptr, &particle_buffer));

	ID3DBlob *blob = compile_shader(L"Resources/Particle.hlsl", "VS", "vs_5_0", device);
	DXCALL(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &particle_vs));

	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "ORIGIN",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SCALE",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATION",  0, DXGI_FORMAT_R32_FLOAT,          0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ROTATIONV", 0, DXGI_FORMAT_R32_FLOAT,          0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "AGE",       0, DXGI_FORMAT_R32_FLOAT,          0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DISTORT",   0, DXGI_FORMAT_R32_FLOAT,          0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TYPE",      0, DXGI_FORMAT_R32_SINT,           0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "IDX",       0, DXGI_FORMAT_R32_SINT,           0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	particle_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, device);

	blob = compile_shader(L"Resources/Particle.hlsl", "GS", "gs_5_0", device);
	DXCALL(device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &particle_gs));

	blob = compile_shader(L"Resources/Particle.hlsl", "PS", "ps_5_0", device);
	DXCALL(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &particle_ps));

	ID3D11Resource *r = nullptr;
	DXCALL(CreateDDSTextureFromFile(device, L"Resources/Particle.dds", &r, &particle_srv, 0, nullptr));

	D3D11_SAMPLER_DESC sadesc;
	ZeroMemory(&sadesc, sizeof(sadesc));
	sadesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sadesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sadesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sadesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	DXCALL(device->CreateSamplerState(&sadesc, &particle_sampler));

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

	state.RenderTarget[1].BlendEnable = FALSE;
	state.RenderTarget[1].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	state.RenderTarget[1].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	state.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
	state.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	state.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	state.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	DXCALL(device->CreateBlendState(&state, &particle_blend));

	state.RenderTarget[0].BlendEnable = FALSE;
	state.RenderTarget[1].BlendEnable = FALSE;
	DXCALL(device->CreateBlendState(&state, &no_blend));

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

	DXCALL(device->CreateBuffer(&desc, &data, &composite_vertex_buffer));

	ID3DBlob *blob = compile_shader(L"Resources/Composite.hlsl", "VS", "vs_5_0", device);
	DXCALL(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &composite_vs));

	D3D11_INPUT_ELEMENT_DESC input_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	composite_layout = create_input_layout(input_desc, ARRAYSIZE(input_desc), blob, device);

	blob = compile_shader(L"Resources/Composite.hlsl", "PS", "ps_5_0", device);
	DXCALL(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &composite_ps));

	D3D11_SAMPLER_DESC sampdesc;
	ZeroMemory(&sampdesc, sizeof(sampdesc));
	sampdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	DXCALL(device->CreateSamplerState(&sampdesc, &composite_sampler));


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

	DXCALL(device->CreateTexture2D(&rtv_desc, nullptr, &dtex));

	DXCALL(device->CreateShaderResourceView(dtex, nullptr, &distort_srv));
	DXCALL(device->CreateRenderTargetView(dtex, nullptr, &distort_rtv));
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::ProcessFX(ParticleEffect &fx, XMMATRIX model, float dt)
{
	auto time = fx.clamp_children ? fx.children_time : fx.time;

	fx.age += dt;
	if (fx.age >= time) {
		
	}
	else {
		for (int i = 0; i < fx.fx_count; ++i) {
			auto &entry = fx.fx[i];

			auto def = particle_definitions[entry.idx];

			if (fx.age > entry.start && fx.age < entry.start + entry.end) {
				auto factor = (fx.age - entry.start) / (entry.end);

				auto spawn_ease = GetEaseFunc(entry.spawn_fn);
				float spawn = spawn_ease((float)entry.spawn_start, (float)entry.spawn_end, factor) * dt;

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

void ParticleSystem::AddFX(std::string name, XMMATRIX model)
{
	auto result = std::find_if(effect_definitions.begin(), effect_definitions.end(), [name](ParticleEffect &a) {
		return std::strcmp(name.c_str(), a.name) == 0;
	});

	if (result != effect_definitions.end())
		effects.push_back(*result);
}

void ParticleSystem::update(Camera *cam, float dt)
{
	for (auto &fx : effects) {
		auto time = fx.clamp_children ? fx.children_time : fx.time;

		fx.age += dt;
		if (fx.age >= time) {

		}
		else {
			for (int i = 0; i < fx.fx_count; ++i) {
				auto &entry = fx.fx[i];

				auto def = particle_definitions[entry.idx];

				if (fx.age > entry.start && fx.age < entry.start + entry.end) {
					auto factor = (fx.age - entry.start) / (entry.end);

					auto spawn_ease = GetEaseFunc(entry.spawn_fn);
					float spawn = spawn_ease((float)entry.spawn_start, (float)entry.spawn_end, factor) * dt;

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

		ParticleDefinition *def = &particle_definitions[p->idx];

		auto age = p->age / def->lifetime;
		//if (!settings.ParticlePaused) {
			p->pos += p->velocity * dt;
			p->velocity -= { 0.f, def->gravity * dt, 0.f, 0.f };
			p->rotation += p->rotation_velocity * dt;
			p->age += dt;
		//}

		if (def->orientation == ParticleOrientation::Velocity) {
			p->origin = p->pos;
		}

		auto scale_fn = GetEaseFunc(def->scale_fn);
		p->scale = {
			scale_fn(def->scale_start, def->scale_end, age) * (def->u2 / 2048.f),
			scale_fn(def->scale_start, def->scale_end, age) * (def->v2 / 2048.f)
		};

		auto distort_fn = GetEaseFunc(def->distort_fn);
		if (distort_fn)
			p->distort = distort_fn(def->distort_start, def->distort_end, age);

		p->uv = { def->u / 2048.f, def->v / 2048.f, (def->u + def->u2) / 2048.f, (def->v + def->v2) / 2048.f };

		auto color_fn = GetEaseFuncV(def->color_fn);
		if (color_fn) {
			p->color = color_fn(
				XMLoadFloat4(&def->start_color),
				XMLoadFloat4(&def->end_color),
				age
			);
		}

		if (p->age > def->lifetime) {
			it = particles.erase(it);
		}
		else {
			it++;
		}
	}

	std::sort(particles.begin(), particles.end(), [cam](Particle &a, Particle &b) {
		return XMVectorGetZ(XMVector3Length(cam->pos - a.pos)) > XMVectorGetZ(XMVector3Length(cam->pos - b.pos));
	});

	if (!particles.empty()) {
		D3D11_MAPPED_SUBRESOURCE data;
		DXCALL(cxt->Map(particle_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data));
		{
			CopyMemory(data.pData, particles.data(), sizeof(Particle) * particles.size());
		}
		cxt->Unmap(particle_buffer, 0);
	}
}

void ParticleSystem::render(Camera *cam, ID3D11RenderTargetView *dst_rtv, ID3D11ShaderResourceView *dst_srv)
{
	{
		UINT32 stride = sizeof(Particle);
		UINT32 offset = 0u;

		cxt->IASetInputLayout(particle_layout);
		cxt->IASetVertexBuffers(0, 1, &particle_buffer, &stride, &offset);
		cxt->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		cxt->VSSetShader(particle_vs, nullptr, 0);

		cxt->GSSetShader(particle_gs, nullptr, 0);
		cxt->GSSetConstantBuffers(0, 1, &cam->wvp_buffer);

		cxt->PSSetShader(particle_ps, nullptr, 0);
		cxt->PSSetConstantBuffers(0, 1, &cam->wvp_buffer);
		cxt->PSSetSamplers(0, 1, &particle_sampler);

		ID3D11ShaderResourceView *particle_srvs[] = {
			particle_srv,
			gDepthbufferSRV
		};
		cxt->PSSetShaderResources(0, 2, particle_srvs);

		ID3D11RenderTargetView *particle_targets[] = {
			dst_rtv,
			distort_rtv
		};
		cxt->OMSetRenderTargets(2, particle_targets, nullptr);
		cxt->OMSetBlendState(particle_blend, nullptr, 0x0);
		cxt->OMSetDepthStencilState(nullptr, 0xff);

		cxt->Draw(particles.size(), 0);

		cxt->PSSetShaderResources(0, 2, RESET_SRV);
		cxt->OMSetRenderTargets(2, RESET_RTV, nullptr);
		cxt->GSSetShader(nullptr, nullptr, 0);
		cxt->OMSetDepthStencilState(gDepthReadWrite, 0xff);
	}

	{
		UINT32 stride = sizeof(float) * 4;
		UINT32 offset = 0u;

		cxt->IASetInputLayout(composite_layout);
		cxt->IASetVertexBuffers(0, 1, &composite_vertex_buffer, &stride, &offset);
		cxt->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cxt->VSSetShader(composite_vs, nullptr, 0);

		cxt->PSSetShader(composite_ps, nullptr, 0);
		cxt->PSSetSamplers(0, 1, &composite_sampler);
		ID3D11ShaderResourceView *composite_srvs[] = {
			dst_srv,
			distort_srv
		};
		cxt->PSSetShaderResources(0, 2, composite_srvs);

		cxt->OMSetBlendState(no_blend, nullptr, 0x0);
		cxt->OMSetRenderTargets(1, &gBackbufferRTV, nullptr);

		cxt->Draw(6, 0);

		cxt->PSSetShaderResources(0, 2, RESET_SRV);
	}
}
