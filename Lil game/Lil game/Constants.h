#pragma once

struct SpellConstants {
	float kArcaneProjectileSeekStrength = 2.0f;
	float kArcaneProjectileSeekRadius = 8.0f;
	float kArcaneProjectileSeekFalloff = 0.08f;
	float kArcaneProjectileStrength = 40.0f;
	float kArcaneProjectileSpeed = 10.0f;
	float kArcaneProjectileCooldown = 0.8f;

	float kArcaneStompDistance = 7.f;
	float kArcaneStompStrength = 5.5f;
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
	float kCameraSpeed = 1.9f;
	float kCameraDrag = 0.9f;
	bool kCanDie = true;
};

extern GameConstants gDefaultGameConstants;
extern GameConstants gGameConstants;