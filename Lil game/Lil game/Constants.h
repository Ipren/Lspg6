#pragma once

struct SpellConstants {
	float kArcaneProjectileSeekStrength = 4.0f;
	float kArcaneProjectileSeekRadius = 5.0f;
	float kArcaneProjectileStrength = 15.0f;
	float kArcaneProjectileSpeed = 10.0f;

	float kArcaneStompDistance = 3.f;
	float kArcaneStompStrength = 5.f;
	float kArcaneStompStrengthFalloff = 2.f;
};

extern SpellConstants gDefaultConstants;
extern SpellConstants gConstants;