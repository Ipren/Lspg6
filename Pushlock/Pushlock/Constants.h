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

	float kFireProjectileStrength = 30.0f;
	float kFireProjectileSpeed = 10.0f;
	float kFireProjectileCooldown = 1.f;
	float kFireProjectileExplosionRadius = 5.f;
	float kFireProjectileExplosionFalloff = 1.f;

	float kFireStompDistance = 5.3f;
	float kFireStompStrength = 5.f;
	float kFireStompStrengthFalloff = 2.f;
	float kFireStompCooldown = 2.5f;

	float kFireDashSpeed = 30.f;
	float kFireDashCooldown = 1.5f;

	float kFireWallCooldown = 4.5f;
	int   kFireWallNrOfPillars = 6;
	float kFireWallPillarDistance = 0.65f;
	float kFireWallPillarRadius = 0.35f;

	float kWindProjectileStrength = 10.0f;
	float kWindProjectileSpeed = 30.0f;
	float kWindProjectileCooldown = 0.1f;

	float kWindStompDistance = 5.3f;
	float kWindStompStrength = 5.f;
	float kWindStompStrengthFalloff = 2.f;
	float kWindStompCooldown = 2.5f;

	float kWindDashSpeed = 30.f;
	float kWindDashCooldown = 1.5f;

	float kWindWallCooldown = 4.5f;
	int   kWindWallNrOfPillars = 6;
	float kWindWallPillarDistance = 0.65f;
	float kWindWallPillarRadius = 0.35f;

	float kEarthProjectileStrength = 0.25f;
	float kEarthProjectileSpeed = 10.0f;
	float kEarthProjectileCooldown = 0.1f;
	float kEarthProjectileEffectRadius = 5.f;
	float kEarthProjectileEffectFalloff = 0.5f;
	float kEarthProjectileEffectArmingTime = 0.5f;

	float kEarthStompDistance = 5.3f;
	float kEarthStompStrength = 5.f;
	float kEarthStompStrengthFalloff = 2.f;
	float kEarthStompCooldown = 2.5f;

	float kEarthDashSpeed = 30.f;
	float kEarthDashCooldown = 1.5f;

	float kEarthWallCooldown = 4.5f;
	int   kEarthWallNrOfPillars = 6;
	float kEarthWallPillarDistance = 0.65f;
	float kEarthWallPillarRadius = 0.35f;

	float kWaterProjectileStrenght = 10.0f;
	float kWaterProjectileSpeed = 10.0f;
	float kWaterProjectileCooldown = 1.0f;
	int kWaterProjectileNrOfShards = 4;
	float kWaterProjectileSpreadAngle = 5.0f;

	float kWaterStompDistance = 5.3f;
	float kWaterStompStrenght = 5.0f;
	float kWaterStompStrenghtFalloff = 2.0f;
	float kWaterStompCooldown = 2.5f;

	float kWaterDashSpeed = 30.0f;
	float kWaterDashCooldonw = 1.5f;

	float kWaterWallCooldown = 4.5f;
	int kWaterWallNrOfPillars = 6;
	float kWaterWallPillarDistance = 0.65f;
	float kWaterWallPillarRadius = 0.35f;
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

struct MapConstants
{
	float kShrinkAmount = 1.0f;
	float kShrinkTimer = 20.0f;
};
extern MapConstants gDefaultMapConstants;
extern MapConstants gMapConstants;