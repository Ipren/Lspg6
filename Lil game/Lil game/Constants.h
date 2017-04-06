#pragma once

struct SpellConstants {
	float kArcaneProjectileSeekStrength = 4.0f;
	float kArcaneProjectileSeekRadius = 5.0f;
	float kArcaneProjectileStrength = 15.0f;
	float kArcaneProjectileSpeed = 10.0f;

	float kArcaneStompDistance = 3.f;
	float kArcaneStompStrength = 5.f;
	float kArcaneStompStrengthFalloff = 2.f;

	float kArcaneDashSpeed = 30.f;
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