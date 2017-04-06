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

	//velocity.x *= 0.9;
	//velocity.y *= 0.9;

	//acceleration.x *= 0.9;
	//acceleration.y *= 0.9;

	if (gGamepads[index]->get_button_down(Gamepad::Rb)) {
		this->element->projectile(this, map);
	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Lstick))
	{
		
		auto left_angle = gGamepads[index]->get_left_thumb_angle();
		this->velocity.x += cos(left_angle) * gSpellConstants.kArcaneDashSpeed;
		this->velocity.y += sin(left_angle) * gSpellConstants.kArcaneDashSpeed;
	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Lb))
	{
		this->element->stomp(this, map);
	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Rt))
	{
		this->element->wall(this, map);
	}


	if (sqrt(this->position.x*this->position.x + this->position.z*this->position.z) > 15)
	{
		this->dead = true;
	}

}
