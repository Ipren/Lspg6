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

	float kEarthProjectileStrength = 0.40f;
	float kEarthProjectileSpeed = 10.0f;
	float kEarthProjectileCooldown = 0.5f;
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
	float kWaterDashCooldown = 1.5f;

	float kWaterWallCooldown = 4.5f;
	int kWaterWallNrOfPillars = 6;
	float kWaterWallPillarDistance = 0.65f;
	float kWaterWallPillarRadius = 0.35f;
};

extern SpellConstants gDefaultSpellConstants;
extern SpellConstants gSpellConstants;


struct PlayerSpellConstants {
	float kArcaneProjectileSeekStrength = 0.0f;
	float kArcaneProjectileSeekRadius = 0.0f;
	float kArcaneProjectileSeekFalloff = 0.0f;
	float kArcaneProjectileStrength = 0.0f;
	float kArcaneProjectileSpeed = 0.0f;
	float kArcaneProjectileCooldown = 0.0f;

	float kArcaneStompDistance = 0.0f;
	float kArcaneStompStrength = 0.0f;
	float kArcaneStompStrengthFalloff = 0.0f;
	float kArcaneStompCooldown = 0.0f;

	float kArcaneDashSpeed = 0.0f;
	float kArcaneDashCooldown = 0.0f;

	float kArcaneWallCooldown = 0.0f;
	int kArcaneWallNrOfPillars = 0.0;
	float kArcaneWallPillarDistance = 0.0f;
	float kArcaneWallPillarRadius = 0.0f;

	float kFireProjectileStrength = 0.0f;
	float kFireProjectileSpeed = 0.0f;
	float kFireProjectileCooldown = 0.0f;
	float kFireProjectileExplosionRadius = 0.0f;
	float kFireProjectileExplosionFalloff = 0.0f;

	float kFireStompDistance = 0.0f;
	float kFireStompStrength = 0.0f;
	float kFireStompStrengthFalloff = 0.0f;
	float kFireStompCooldown = 0.0f;

	float kFireDashSpeed = 0.0f;
	float kFireDashCooldown = 0.0f;

	float kFireWallCooldown = 0.0f;
	int   kFireWallNrOfPillars = 0.0f;
	float kFireWallPillarDistance = 0.0f;
	float kFireWallPillarRadius = 0.0f;

	float kWindProjectileStrength = 0.0f;
	float kWindProjectileSpeed = 0.0f;
	float kWindProjectileCooldown = 0.0f;

	float kWindStompDistance = 0.0f;
	float kWindStompStrength = 0.0f;
	float kWindStompStrengthFalloff = 0.0f;
	float kWindStompCooldown = 0.0f;

	float kWindDashSpeed = 0.0f;
	float kWindDashCooldown = 0.0f;

	float kWindWallCooldown = 0.0f;
	int   kWindWallNrOfPillars = 0.0f;
	float kWindWallPillarDistance = 0.0f;
	float kWindWallPillarRadius = 0.0f;

	float kEarthProjectileStrength = 0.0f;
	float kEarthProjectileSpeed = 0.0f;
	float kEarthProjectileCooldown = 0.0f;
	float kEarthProjectileEffectRadius = 0.0f;
	float kEarthProjectileEffectFalloff = 0.0f;
	float kEarthProjectileEffectArmingTime = 0.0f;

	float kEarthStompDistance = 0.0f;
	float kEarthStompStrength = 0.0f;
	float kEarthStompStrengthFalloff = 0.0f;
	float kEarthStompCooldown = 0.0f;

	float kEarthDashSpeed = 0.0f;
	float kEarthDashCooldown = 0.0f;

	float kEarthWallCooldown = 0.0f;
	int   kEarthWallNrOfPillars = 0.0f;
	float kEarthWallPillarDistance = 0.0f;
	float kEarthWallPillarRadius = 0.0f;

	float kWaterProjectileStrenght = 0.0f;
	float kWaterProjectileSpeed = 0.0f;
	float kWaterProjectileCooldown = 0.0f;
	int kWaterProjectileNrOfShards = 0.0f;
	float kWaterProjectileSpreadAngle = 0.0f;

	float kWaterStompDistance = 0.0f;
	float kWaterStompStrenght = 0.0f;
	float kWaterStompStrenghtFalloff = 0.0f;
	float kWaterStompCooldown = 0.0f;

	float kWaterDashSpeed = 0.0f;
	float kWaterDashCooldown = 0.0f;

	float kWaterWallCooldown = 0.0f;
	int kWaterWallNrOfPillars = 0.0f;
	float kWaterWallPillarDistance = 0.0f;
	float kWaterWallPillarRadius = 0.0f;

	//void addDefault(SpellConstants def)
	//{
	//	kArcaneProjectileSeekStrength += def.kArcaneProjectileSeekStrength;
	//	kArcaneProjectileSeekRadius += def.kArcaneProjectileSeekRadius;
	//	kArcaneProjectileSeekFalloff += def.kArcaneProjectileSeekFalloff;
	//	kArcaneProjectileStrength += def.kArcaneProjectileStrength;
	//	kArcaneProjectileSpeed += def.kArcaneProjectileSpeed;
	//	kArcaneProjectileCooldown += def.kArcaneProjectileCooldown;

	//	kArcaneStompDistance += def.kArcaneStompDistance;
	//	kArcaneStompStrength += def.kArcaneStompStrength;
	//	kArcaneStompStrengthFalloff += def.kArcaneStompStrengthFalloff;
	//	kArcaneStompCooldown += def.kArcaneStompCooldown;

	//	kArcaneDashSpeed += def.kArcaneDashSpeed;
	//	kArcaneDashCooldown += def.kArcaneDashCooldown;

	//	kArcaneWallCooldown += def.kArcaneWallCooldown;
	//	kArcaneWallNrOfPillars += def.kArcaneWallNrOfPillars;
	//	kArcaneWallPillarDistance += def.kArcaneWallPillarDistance;
	//	kArcaneWallPillarRadius += def.kArcaneWallPillarRadius;

	//	kFireProjectileStrength += def.kFireProjectileStrength;
	//	kFireProjectileSpeed += def.kFireProjectileSpeed;
	//	kFireProjectileCooldown += def.kFireProjectileCooldown;
	//	kFireProjectileExplosionRadius += def.kFireProjectileExplosionRadius;
	//	kFireProjectileExplosionFalloff += def.kFireProjectileExplosionFalloff;

	//	kFireStompDistance += def.kFireStompDistance;
	//	kFireStompStrength += def.kFireStompStrength;
	//	kFireStompStrengthFalloff += def.kFireStompStrengthFalloff;
	//	kFireStompCooldown += def.kFireStompCooldown;

	//	kFireDashSpeed += def.kFireDashSpeed;
	//	kFireDashCooldown += def.kFireDashCooldown;

	//	kFireWallCooldown += def.kFireWallCooldown;
	//	kFireWallNrOfPillars += def.kFireWallNrOfPillars;
	//	kFireWallPillarDistance += def.kFireWallPillarDistance;
	//	kFireWallPillarRadius += def.kFireWallPillarRadius;

	//	kWindProjectileStrength += def.kWindProjectileStrength;
	//	kWindProjectileSpeed += def.kWindProjectileSpeed;
	//	kWindProjectileCooldown += def.kWindProjectileCooldown;

	//	kWindStompDistance += def.kWindStompDistance;
	//	kWindStompStrength += def.kWindStompStrength;
	//	kWindStompStrengthFalloff += def.kWindStompStrengthFalloff;
	//	kWindStompCooldown += def.kWindStompCooldown;

	//	kWindDashSpeed += def.kWindDashSpeed;
	//	kWindDashCooldown += def.kWindDashCooldown;

	//	kWindWallCooldown += def.kWindWallCooldown;
	//	kWindWallNrOfPillars += def.kWindWallNrOfPillars;
	//	kWindWallPillarDistance += def.kWindWallPillarDistance;
	//	kWindWallPillarRadius += def.kWindWallPillarRadius;

	//	kEarthProjectileStrength += def.kEarthProjectileStrength;
	//	kEarthProjectileSpeed += def.kEarthProjectileSpeed;
	//	kEarthProjectileCooldown += def.kEarthProjectileCooldown;
	//	kEarthProjectileEffectRadius += def.kEarthProjectileEffectRadius;
	//	kEarthProjectileEffectFalloff += def.kEarthProjectileEffectFalloff;
	//	kEarthProjectileEffectArmingTime += def.kEarthProjectileEffectArmingTime;

	//	kEarthStompDistance += def.kEarthStompDistance;
	//	kEarthStompStrength += def.kEarthStompStrength;
	//	kEarthStompStrengthFalloff += def.kEarthStompStrengthFalloff;
	//	kEarthStompCooldown += def.kEarthStompCooldown;

	//	kEarthDashSpeed += def.kEarthDashSpeed;
	//	kEarthDashCooldown += def.kEarthDashCooldown;

	//	kEarthWallCooldown += def.kEarthWallCooldown;
	//	kEarthWallNrOfPillars += def.kEarthWallNrOfPillars;
	//	kEarthWallPillarDistance += def.kEarthWallPillarDistance;
	//	kEarthWallPillarRadius += def.kEarthWallPillarRadius;

	//	kWaterProjectileStrenght += def.kWaterProjectileStrenght;
	//	kWaterProjectileSpeed += def.kWaterProjectileSpeed;
	//	kWaterProjectileCooldown += def.kWaterProjectileCooldown;
	//	kWaterProjectileNrOfShards += def.kWaterProjectileNrOfShards;
	//	kWaterProjectileSpreadAngle += def.kWaterProjectileSpreadAngle;

	//	kWaterStompDistance += def.kWaterStompDistance;
	//	kWaterStompStrenght += def.kWaterStompStrenght;
	//	kWaterStompStrenghtFalloff += def.kWaterStompStrenghtFalloff;
	//	kWaterStompCooldown += def.kWaterStompCooldown;

	//	kWaterDashSpeed += def.kWaterDashSpeed;
	//	kWaterDashCooldown += def.kWaterDashCooldown;

	//	kWaterWallCooldown += def.kWaterWallCooldown;
	//	kWaterWallNrOfPillars += def.kWaterWallNrOfPillars;
	//	kWaterWallPillarDistance += def.kWaterWallPillarDistance;
	//	kWaterWallPillarRadius += def.kWaterWallPillarRadius;

	//	//Just in case we need
	//	/*kArcaneProjectileSeekStrength = 0.0f;
	//	kArcaneProjectileSeekRadius = 0.0f;
	//	kArcaneProjectileSeekFalloff = 0.0f;
	//	kArcaneProjectileStrength = 0.0f;
	//	kArcaneProjectileSpeed = 0.0f;
	//	kArcaneProjectileCooldown = 0.0f;

	//	kArcaneStompDistance = 0.0f;
	//	kArcaneStompStrength = 0.0f;
	//	kArcaneStompStrengthFalloff = 0.0f;
	//	kArcaneStompCooldown = 0.0f;

	//	kArcaneDashSpeed = 0.0f;
	//	kArcaneDashCooldown = 0.0f;

	//	kArcaneWallCooldown = 0.0f;
	//	kArcaneWallNrOfPillars = 0.0;
	//	kArcaneWallPillarDistance = 0.0f;
	//	kArcaneWallPillarRadius = 0.0f;

	//	kFireProjectileStrength = 0.0f;
	//	kFireProjectileSpeed = 0.0f;
	//	kFireProjectileCooldown = 0.0f;
	//	kFireProjectileExplosionRadius = 0.0f;
	//	kFireProjectileExplosionFalloff = 0.0f;

	//	kFireStompDistance = 0.0f;
	//	kFireStompStrength = 0.0f;
	//	kFireStompStrengthFalloff = 0.0f;
	//	kFireStompCooldown = 0.0f;

	//	kFireDashSpeed = 0.0f;
	//	kFireDashCooldown = 0.0f;

	//	kFireWallCooldown = 0.0f;
	//	kFireWallNrOfPillars = 0.0f;
	//	kFireWallPillarDistance = 0.0f;
	//	kFireWallPillarRadius = 0.0f;

	//	kWindProjectileStrength = 0.0f;
	//	kWindProjectileSpeed = 0.0f;
	//	kWindProjectileCooldown = 0.0f;

	//	kWindStompDistance = 0.0f;
	//	kWindStompStrength = 0.0f;
	//	kWindStompStrengthFalloff = 0.0f;
	//	kWindStompCooldown = 0.0f;

	//	kWindDashSpeed = 0.0f;
	//	kWindDashCooldown = 0.0f;

	//	kWindWallCooldown = 0.0f;
	//	kWindWallNrOfPillars = 0.0f;
	//	kWindWallPillarDistance = 0.0f;
	//	kWindWallPillarRadius = 0.0f;

	//	kEarthProjectileStrength = 0.0f;
	//	kEarthProjectileSpeed = 0.0f;
	//	kEarthProjectileCooldown = 0.0f;
	//	kEarthProjectileEffectRadius = 0.0f;
	//	kEarthProjectileEffectFalloff = 0.0f;
	//	kEarthProjectileEffectArmingTime = 0.0f;

	//	kEarthStompDistance = 0.0f;
	//	kEarthStompStrength = 0.0f;
	//	kEarthStompStrengthFalloff = 0.0f;
	//	kEarthStompCooldown = 0.0f;

	//	kEarthDashSpeed = 0.0f;
	//	kEarthDashCooldown = 0.0f;

	//	kEarthWallCooldown = 0.0f;
	//	kEarthWallNrOfPillars = 0.0f;
	//	kEarthWallPillarDistance = 0.0f;
	//	kEarthWallPillarRadius = 0.0f;

	//	kWaterProjectileStrenght = 0.0f;
	//	kWaterProjectileSpeed = 0.0f;
	//	kWaterProjectileCooldown = 0.0f;
	//	kWaterProjectileNrOfShards = 0.0f;
	//	kWaterProjectileSpreadAngle = 0.0f;

	//	kWaterStompDistance = 0.0f;
	//	kWaterStompStrenght = 0.0f;
	//	kWaterStompStrenghtFalloff = 0.0f;
	//	kWaterStompCooldown = 0.0f;

	//	kWaterDashSpeed = 0.0f;
	//	kWaterDashCooldown = 0.0f;

	//	kWaterWallCooldown = 0.0f;
	//	kWaterWallNrOfPillars = 0.0f;
	//	kWaterWallPillarDistance = 0.0f;
	//	kWaterWallPillarRadius = 0.0f;*/
	//}
};

extern PlayerSpellConstants gDefaultPlayerSpellConstants[4];
extern PlayerSpellConstants gPlayerSpellConstants[4];

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