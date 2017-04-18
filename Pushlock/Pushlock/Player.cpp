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

	element = new ArcaneElement;

	for (int i = 0; i < 5; i++)
	{
		this->cooldown[i] = 0;
	}
	stomped = false;
	blowUp = false;
	ready = false;
}

Player::~Player()
{
	delete element;
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

	if (*map->currentState == GameState::Playing)
	{
		this->element->update(this, map, dt);
		if ((dynamic_cast<WindElement*>(element) != nullptr || dynamic_cast<ArcaneElement*>(element) != nullptr) && gGamepads[index]->get_button_down(Gamepad::Rb)) {//projectile
			this->element->projectile(this, map);
		}
		else if (gGamepads[index]->get_button_pressed(Gamepad::Rb)) {//projectile
			this->element->projectile(this, map);
		}

		if (gGamepads[index]->get_button_pressed(Gamepad::Lt))//dash
		{
			this->element->dash(this, map);
		}

		if (gGamepads[index]->get_button_pressed(Gamepad::Lb))//stomp
		{
			this->element->stomp(this, map);
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
	if (*map->currentState == GameState::ChoosePowers)
	{
		if (gGamepads[index]->get_button_pressed(Gamepad::A))
		{
			delete element;
			element = new ArcaneElement();
			map->playerElemnts[index] = 0;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::B))
		{
			delete element;
			element = new FireElement();
			map->playerElemnts[index] = 1;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::X))
		{
			delete element;
			element = new WindElement();
			map->playerElemnts[index] = 2;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::Y))
		{
			delete element;
			element = new EarthElement();
			map->playerElemnts[index] = 3;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::Rb))
		{
			delete element;
			element = new WaterElement();
			map->playerElemnts[index] = 4;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::Start))
		{
			this->ready = true;
		}

	}

}
