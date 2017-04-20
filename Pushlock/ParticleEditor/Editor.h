#pragma once

namespace Editor {

struct Settings {
	float CameraDistance = 4.785f;
	float CameraHeight = 4.875f;
	float CameraSpeed = 0.075f;

	float ParticleSpeed = 1.0f;
	bool ParticlePaused = false;
	bool ParticleLoop = false;
};

void Init();
void Update(float dt);
void Render(float dt);

}