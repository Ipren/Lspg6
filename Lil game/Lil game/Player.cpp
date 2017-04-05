#include "Player.h"

#include "Spell.h"
#include "Globals.h"
#include "Gamepad.h"

Player::Player(unsigned int index, XMFLOAT3 position, XMFLOAT2 velocity, float radius) :
	Entity(EntityType::Player, position, velocity, radius), index(index)
{
	this->velocity.x = 0;
	this->velocity.y = 0;
}

Player::~Player()
{
}

void Player::update(Map *map, float dt)
{
	auto left = gGamepads[index]->get_left_thumb();
	auto right_angle = gGamepads[index]->get_right_thumb_angle();
	angle = right_angle;

	acceleration.x += left.x * 30;
	acceleration.y += left.y * 30;

	velocity.x += acceleration.x * dt;
	velocity.y += acceleration.y * dt;

	position.x += velocity.x * dt;
	position.z += velocity.y * dt;


	velocity.x -= velocity.x * 5 * dt;
	velocity.y -= velocity.y * 5 * dt;

	acceleration.x = 0;
	acceleration.y = 0;

	//velocity.x *= 0.9;
	//velocity.y *= 0.9;

	//acceleration.x *= 0.9;
	//acceleration.y *= 0.9;

	if (gGamepads[index]->get_button_pressed(Gamepad::Rb)) {
		PushSpell *spell = new PushSpell({ 
				position.x + cos(angle) * (radius + 0.4f),
				0,
				position.z + sin(angle) * (radius + 0.4f)
			},
			{ cos(angle) * 30, sin(angle) * 30 },
			0.1
		);
		map->add_entity(spell);
	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Lstick))
	{
		
		auto left_angle = gGamepads[index]->get_left_thumb_angle();
		this->velocity.x += cos(left_angle) * 30;
		this->velocity.y += sin(left_angle) * 30;
	}

	if (gGamepads[index]->get_button_pressed(Gamepad::Lb))
	{
		auto nearby = map->get_entities_in_radius(this, 3);

		for (auto result : nearby) {
			result.entity->velocity.x += cos(result.angle) * 10 * abs(5 - result.distance);
			result.entity->velocity.y += sin(result.angle) * 10 * abs(5 - result.distance);
		}
	}
	if (gGamepads[index]->get_button_pressed(Gamepad::Rt))
	{
		XMVECTOR pos = {
			position.x + cos(angle) * (radius + 0.9f),
			0,
			position.z + sin(angle) * (radius + 0.9f)
		};

		XMVECTOR dist = pos - XMLoadFloat3(&position);
		XMVECTOR n = XMVector3Cross(dist, { 0, 1, 0 });

		for (int i = 0; i < 6; i++)
		{

			XMFLOAT3 p;
			XMStoreFloat3(&p, n * (i - 3)*0.35 + pos);
			map->add_entity(new WallSpell(p,0.35));
		}
	}


	if (sqrt(this->position.x*this->position.x + this->position.z*this->position.z) > 15)
	{
		this->dead = true;
	}

}
