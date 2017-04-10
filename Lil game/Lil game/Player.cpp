#include "Player.h"

#include "Spell.h"
#include "Globals.h"
#include "Gamepad.h"
#include "Constants.h"

Player::Player(unsigned int index, XMFLOAT3 position, XMFLOAT2 velocity, float radius) :
	Entity(EntityType::Player, position, velocity, radius), index(index)
{
	this->velocity.x = 0;
	this->velocity.y = 0;

	element = new ArcaneElement();
	for (int i = 0; i < 5; i++)
	{
		this->cooldown[i] = 0;
	}
	stomped = false;
}

Player::~Player()
{
}

void Player::update(Map *map, float dt)
{

	auto left = gGamepads[index]->get_left_thumb();
	auto right_angle = gGamepads[index]->get_right_thumb_angle();
	angle = right_angle;

	acceleration.x += left.x * gPlayerConstants.kSpeed;
	acceleration.y += left.y * gPlayerConstants.kSpeed;

	velocity.x += acceleration.x * dt;
	velocity.y += acceleration.y * dt;

	position.x += velocity.x * dt;
	position.z += velocity.y * dt;


	velocity.x -= velocity.x * gPlayerConstants.kFriction * dt;
	velocity.y -= velocity.y * gPlayerConstants.kFriction * dt;

	acceleration.x = 0;
	acceleration.y = 0;
	stomped = false;

	//velocity.x *= 0.9;
	//velocity.y *= 0.9;

	//acceleration.x *= 0.9;
	//acceleration.y *= 0.9;

	for (int i = 0; i < 5; i++)//counting down all cooldowns
	{
		if (cooldown[i]>0)
			this->cooldown[i] -= dt;
		if (cooldown[i] < 0)
			this->cooldown[i] = 0;
	}

	if (gGamepads[index]->get_button_down(Gamepad::Rb) && cooldown[0] == 0.f) {//projectile
		this->element->projectile(this, map);
		this->cooldown[0] = gSpellConstants.kArcaneProjectileCooldown;//cooldown time
	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Lt) && cooldown[1] == 0.f)//dash
	{
		XMFLOAT2 leftVector = gGamepads[index]->get_left_thumb();

		//check if left stick is centered
		if (leftVector.x != 0.f && leftVector.y != 0.f)//if it is not: dash to where you are walking
		{
			float left_angle = gGamepads[index]->get_left_thumb_angle();
			this->velocity.x += cos(left_angle) * gSpellConstants.kArcaneDashSpeed;
			this->velocity.y += sin(left_angle) * gSpellConstants.kArcaneDashSpeed;
		}
		else//if it is centered: dash to where you are looking
		{
			this->velocity.x += cos(this->angle) * gSpellConstants.kArcaneDashSpeed;
			this->velocity.y += sin(this->angle) * gSpellConstants.kArcaneDashSpeed;
		}
		

		this->cooldown[1] = gSpellConstants.kArcaneDashCooldown;//cooldown time

	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Lb) && cooldown[2] == 0.f)//stomp
	{
		this->element->stomp(this, map);
		this->cooldown[2] = gSpellConstants.kArcaneStompCooldown;//cooldown time
		stomped = true;
		
	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Rt) && cooldown[3] == 0.f)//wall
	{
		this->element->wall(this, map);
		this->cooldown[3] = gSpellConstants.kArcaneWallCooldown;//cooldown time

	}


	if (sqrt(this->position.x*this->position.x + this->position.z*this->position.z) > map->radius && gGameConstants.kCanDie)
	{
		this->dead = true;
	}

}
