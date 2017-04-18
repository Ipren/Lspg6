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

	ParticleEmitter emitter_type;
	float emitter_xmin, emitter_xmax;

	ParticleEase spawn_fn;
	int spawn_start, spawn_end;
};

struct ParticleEffectEntry {
	ParticleDefinition *def;
	float start, end;
};

struct ParticleEffect {
	char name[32] = { "Untitled\0" };
	ParticleEffectEntry fx[MAX_PARTICLE_FX];
	bool loop;
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