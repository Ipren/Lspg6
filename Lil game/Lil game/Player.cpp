#include "Player.h"

#include "Globals.h"

Player::Player(unsigned int index) :
	index(index)
{
	this->velocity.x = 0;
	this->velocity.y = 0;
}

Player::~Player()
{
}

void Player::update(float dt)
{
	auto left = gGamepads[index]->get_left_thumb();
	auto right_angle = gGamepads[index]->get_right_thumb_angle();

	velocity.x += left.x * 10.f;
	velocity.y += left.y * 10.f;

	angle = right_angle;

	position.x += velocity.x*dt;
	position.z += velocity.y*dt;

	velocity.x = 0;
	velocity.y = 0;
}
