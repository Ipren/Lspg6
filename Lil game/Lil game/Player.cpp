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

	//element = new ArcaneElement();
	element = new FireElement();
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

	this->element->update(this, map, dt);

	if (gGamepads[index]->get_button_pressed(Gamepad::Rb)) {//projectile
		this->element->projectile(this, map);
	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Lt))//dash
	{
		this->element->dash(this, map);
	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Lb))//stomp
	{
		this->element->stomp(this, map);
		stomped = true;
	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Rt))//wall
	{
		this->element->wall(this, map);
	}

	if (sqrt(this->position.x*this->position.x + this->position.z*this->position.z) > map->radius && gGameConstants.kCanDie)
	{
		this->dead = true;
		map->nrOfAlivePlayers--;
	}

}
