#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include <DirectXMath.h>
#include "Ease.h"
#include "Camera.h"

using namespace DirectX;

const char *EMITTER_STRINGS = "Static\0Box\0Sphere\0";

enum class ParticleEmitter {
	Static = 0,
	Cube,
	Sphere
};

enum class ParticleOrientation {
	Planar = 0,
	Clip,
	Velocity,
	VelocityAnchored
};

enum class ParticleEase {
	Linear = 0,
	EaseIn,
	EaseOut,
	None
};

typedef float(*EaseFunc)(float, float, float);
typedef XMVECTOR(*EaseFuncV)(XMVECTOR, XMVECTOR, float);

EaseFunc ease_funcs[] = {
	ease::Lerp,
	ease::EaseIn,
	ease::EaseOut,
	nullptr
};

EaseFuncV ease_funcs_xmv[] = {
	ease::Lerp,
	ease::EaseIn,
	ease::EaseOut,
	nullptr
};

const char *EASE_STRINGS = "Linear\0EaseIn\0EaseOut\0";
const char *EASE_STRINGS_OPTIONAL = "Linear\0EaseIn\0EaseOut\0None\0";

EaseFunc GetEaseFunc(ParticleEase ease)
{
	return ease_funcs[(int)ease];
}

EaseFuncV GetEaseFuncV(ParticleEase ease)
{
	return ease_funcs_xmv[(int)ease];
}


#define MAX_PARTICLE_FX 16

struct ParticleDefinition {
	char name[32] = { "Untitled\0" };
	ParticleOrientation orientation;

	float gravity;
	float lifetime = 1.f;

	ParticleEase scale_fn;
	float scale_start;
	float scale_end = 1.0;

	ParticleEase distort_fn = ParticleEase::None;
	float distort_start = 1.0;
	float distort_end;

	ParticleEase color_fn;
	XMFLOAT4 start_color = { 1.f, 1.f, 1.f, 1.f };
	XMFLOAT4 end_color = { 1.f, 1.f, 1.f, 0.f };

	int u, v, u2 = 256, v2 = 256;
};

struct ParticleEffectEntry {
	int idx = -1;
	float start, end = 1.f;
	bool loop;

	ParticleEmitter emitter_type;
	float emitter_xmin, emitter_xmax;
	float emitter_ymin, emitter_ymax;
	float emitter_zmin, emitter_zmax;

	float vel_xmin, vel_xmax;
	float vel_ymin, vel_ymax;
	float vel_zmin, vel_zmax;

	float rot_min, rot_max;
	float rot_vmin, rot_vmax;

	ParticleEase spawn_fn;
	int spawn_start = 0, spawn_end;
	float spawned_particles = 1.f;
};

struct ParticleEffect {
	char name[32] = { "Untitled\0" };
	int fx_count = 0;
	ParticleEffectEntry fx[MAX_PARTICLE_FX];
	float children_time = 0.f;
	float time = 1.f;
	float age;

	bool loop;
	bool clamp_children = false;
};

struct Particle {
	XMVECTOR origin;
	XMVECTOR pos;
	XMVECTOR velocity;
	XMVECTOR color;
	XMFLOAT4 uv;
	XMFLOAT2 scale;

	float rotation;
	float rotation_velocity;
	float age;
	float distort;
	int type;
	int idx;
};

class ParticleSystem {
public:
	ParticleSystem(UINT capacity);
	~ParticleSystem();

	void ProcessFX(ParticleEffect &fx, XMMATRIX model, float dt);
	void AddFX(std::string name, XMMATRIX model);

	void update(Camera *cam, float dt);
	void render(Camera *cam, ID3D11RenderTargetView *dst_rtv, ID3D11ShaderResourceView *dst_srv);

private:
	//ParticleEffect *getEffect(std::string name);

	std::vector<ParticleDefinition> particle_definitions;
	std::vector<ParticleEffect> effect_definitions;

	std::vector<ParticleEffect> effects;
	std::vector<Particle> particles;

	ID3D11BlendState *no_blend;
	ID3D11Buffer *particle_buffer;
	ID3D11InputLayout *particle_layout;

	ID3D11VertexShader *particle_vs;
	ID3D11GeometryShader *particle_gs;
	ID3D11PixelShader *particle_ps;
	ID3D11ShaderResourceView *particle_srv;
	ID3D11SamplerState *particle_sampler;

	ID3D11BlendState *particle_blend;

	ID3D11Buffer *composite_vertex_buffer;
	ID3D11InputLayout *composite_layout;

	ID3D11VertexShader *composite_vs;
	ID3D11PixelShader *composite_ps;
	ID3D11SamplerState *composite_sampler;


	ID3D11RenderTargetView *distort_rtv;
	ID3D11ShaderResourceView *distort_srv;

	ID3D11Device *device;
	ID3D11DeviceContext *cxt;
};

inline bool SerializeParticles(const wchar_t *file, std::vector<ParticleEffect> effects, std::vector<ParticleDefinition> definitions)
{
	FILE *f;
	
	if (_wfopen_s(&f, file, L"wb+") != 0) {
		return false;
	}

	auto size = definitions.size();
	fwrite(&size, sizeof(size_t), 1, f);
	fwrite(definitions.data(), sizeof(ParticleDefinition), definitions.size(), f);

	size = effects.size();
	fwrite(&size, sizeof(size_t), 1, f);
	fwrite(effects.data(), sizeof(ParticleEffect), effects.size(), f);

	fclose(f);

	return true;
}

inline bool DeserializeParticles(const wchar_t *file, std::vector<ParticleEffect> &effects, std::vector<ParticleDefinition> &definitions)
{
	FILE *f;
	
	if (_wfopen_s(&f, file, L"rb") != 0) {
		return false;
	}

	size_t size;
	fread(&size, sizeof(size_t), 1, f);

	for (int i = 0; i < size; ++i) {
		ParticleDefinition e;
		fread(&e, sizeof(ParticleDefinition), 1, f);

		definitions.push_back(e);
	}


	fread(&size, sizeof(size_t), 1, f);

	for (int i = 0; i < size; ++i) {
		ParticleEffect e;
		fread(&e, sizeof(ParticleEffect), 1, f);

		effects.push_back(e);
	}

	fclose(f);

	return true;
}

