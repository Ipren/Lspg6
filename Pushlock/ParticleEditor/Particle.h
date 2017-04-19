#pragma once

#include <DirectXMath.h>
#include "Ease.h"

using namespace DirectX;

const char *EMITTER_STRINGS = "Static\0Cube\0";

enum class ParticleEmitter {
	Static = 0,
	Cube,
};

enum class ParticleOrientation {
	Planar = 0,
	Clip,
	Velocity
};

enum class ParticleEase {
	Linear = 0,
	EaseIn,
	EaseOut
};

typedef float(*EaseFunc)(float, float, float);
typedef XMVECTOR(*EaseFuncV)(XMVECTOR, XMVECTOR, float);

EaseFunc ease_funcs[] = {
	ease::Lerp,
	ease::EaseIn,
	ease::EaseOut
};

EaseFuncV ease_funcs_xmv[] = {
	ease::Lerp,
	ease::EaseIn,
	ease::EaseOut
};

const char *EASE_STRINGS = "Linear\0EaseIn\0EaseOut\0";

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

	float rotation_clamp;
	float rotation_multiplier;

	ParticleEase color_fn;
	XMFLOAT4 start_color = { 1.f, 1.f, 1.f, 1.f };
	XMFLOAT4 end_color = { 1.f, 1.f, 1.f, 0.f };

	int u, v, u2 = 256, v2 = 256;
};

struct ParticleEffectEntry {
	int idx = -1;
	float start, end = 1.f;

	ParticleEmitter emitter_type;
	float emitter_xmin, emitter_xmax;
	float emitter_ymin, emitter_ymax;
	float emitter_zmin, emitter_zmax;

	float vel_xmin, vel_xmax;
	float vel_ymin, vel_ymax;
	float vel_zmin, vel_zmax;

	ParticleEase spawn_fn;
	int spawn_start = 1, spawn_end;
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
	XMVECTOR pos;
	XMVECTOR velocity;
	XMVECTOR color;
	XMFLOAT4 uv;
	XMFLOAT2 scale;

	float age;
	int type;
	int idx;
};

#include <cstdio>

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

