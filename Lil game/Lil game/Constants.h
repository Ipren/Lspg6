#pragma once

struct SpellConstants {
	float kArcaneProjectileSeekStrength = 4.0f;
	float kArcaneProjectileSeekRadius = 5.0f;
	float kArcaneProjectileStrength = 15.0f;
	float kArcaneProjectileSpeed = 10.0f;
	float kArcaneProjectileCooldown = .5f;

	float kArcaneStompDistance = 3.f;
	float kArcaneStompStrength = 5.f;
	float kArcaneStompStrengthFalloff = 2.f;
	float kArcaneStompCooldown = 2.5f;

	float kArcaneDashSpeed = 30.f;
	float kArcaneDashCooldown = 1.5f;

	float kArcaneWallCooldown = 4.5f;
	int kArcaneWallNrOfPillars = 6;
	float kArcaneWallPillarDistance = 0.65f;
	float kArcaneWallPillarRadius = 0.35f;
};

extern SpellConstants gDefaultSpellConstants;
extern SpellConstants gSpellConstants;


struct PlayerConstants {
	float kRadius = 0.5f;
	float kSpeed = 30.0f;
	float kFriction = 5.0f;
};

extern PlayerConstants gDefaultPlayerConstants;
extern PlayerConstants gPlayerConstants;

struct GameConstants {
	float kCameraX = 0.0f;
	float kCameraY = 15.0f;
	float kCameraZ = -5.0f;
	float kCameraSpeed = 0.1f;
	float kCameraDrag = 0.9f;
	bool kCanDie = true;
};

extern GameConstants gDefaultGameConstants;
extern GameConstants gGameConstants;