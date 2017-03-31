#include "Player.h"

#include "Globals.h"

Player::Player(unsigned int index) :
	index(index)
{
}

Player::~Player()
{
}

void Player::update()
{
	auto left = gGamepads[index]->get_left_thumb();
	auto right_angle = gGamepads[index]->get_right_thumb_angle();

	velocity.x = left.x / 256.f;
	velocity.y = left.y / 256.f;

	angle = right_angle;

	position.x += velocity.x;
	position.z += velocity.y;
}
