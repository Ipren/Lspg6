#include "Player.h"

#include "Spell.h"
#include "Globals.h"
#include "Gamepad.h"
#include "Constants.h"
#include "Upgrades.h"

Player::Player(unsigned int index, XMFLOAT3 position, XMFLOAT2 velocity, float radius) :
	Entity(EntityType::Player, position, velocity, radius), index(index)
{
	this->velocity.x = 0;
	this->velocity.y = 0;

	element = new ArcaneElement;

	/*for (int i = 0; i < 5; i++)
	{
		this->cooldown[i] = 0;
	}*/
	stomped = false;
	blowUp = false;
	ready = false;
	frosen = false;
	this->health = gPlayerConstants.maxHealth + gPlayerSpellConstants[index].kHealth;
	this->maxHealth = gPlayerConstants.maxHealth + gPlayerSpellConstants[index].kHealth;

	this->debuffs.dot = 0.0f;
	this->debuffs.speed = 0.0f;
	this->debuffs.duration = 0.0f;
	this->debuffs.friction = 0.0f;
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

	acceleration.x += left.x * (gPlayerConstants.kSpeed + gPlayerSpellConstants[index].kSpeed + (debuffs.speed * debuffs.duration));
	acceleration.y += left.y * (gPlayerConstants.kSpeed + gPlayerSpellConstants[index].kSpeed + (debuffs.speed * debuffs.duration));


	
	if (debuffs.duration  - dt > 0.0f)
	{
		debuffs.duration -= dt;
		this->health += (debuffs.dot * dt);
	}
	else
	{
		this->debuffs.dot = 0.0f;
		this->debuffs.speed = 0.0f;
		this->debuffs.friction = 0.0f;
	}
	dashTime += dt;
	if (dashTime > 0.54f)
	{
		dashing = false;
		frosen = false;
	}
	

	velocity.x += acceleration.x * dt;
	velocity.y += acceleration.y * dt;

	position.x += velocity.x * dt;
	position.z += velocity.y * dt;


	velocity.x -= velocity.x * (gPlayerConstants.kFriction + gPlayerSpellConstants[index].kFriction) * dt;
	velocity.y -= velocity.y * (gPlayerConstants.kFriction + gPlayerSpellConstants[index].kFriction) * dt;

	acceleration.x = 0;
	acceleration.y = 0;
	stomped = false;

	//velocity.x *= 0.9;
	//velocity.y *= 0.9;

	//acceleration.x *= 0.9;
	//acceleration.y *= 0.9;
	if (map->playerElemnts[index] == 4 && pUpgrades[index].choice[1] == 2)
	{
		
		WaterElement *e = dynamic_cast<WaterElement *>(this->element);
		e->time += dt;
		if (e->icePatchCount != 0 && e->time > 0.02f)
		{
			e->icePatchCount--;
			e->time = 0.0f;
			WaterIcePatch *spell = new WaterIcePatch(this, this->position, XMFLOAT2(0.0f, 0.0f), 1.0f);
			map->add_entity(spell);
		}
	}

	if (map->playerElemnts[index] == 1 && pUpgrades[index].choice[1] == 1)
	{

		FireElement *e = dynamic_cast<FireElement *>(this->element);
		e->time += dt;
		if (e->firePatchCount != 0 && e->time > 0.02f)
		{
			e->firePatchCount--;
			e->time = 0.0f;
			FirePathSpell *spell = new FirePathSpell(this, this->position, XMFLOAT2(0.0f, 0.0f), 0.4f);
			map->add_entity(spell);
		}

	}

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
			this->health -= 0.01f;
			if (this->health <= 0) 
			{
				this->dead = true;
				map->nrOfAlivePlayers--;
				map->sounds.play(spellSounds::playerDeath, 0.0f, 50.0f);
			}
		}
	}
	if (*map->currentState == GameState::ChoosePowers)
	{
		if (gGamepads[index]->get_button_pressed(Gamepad::A))
		{

			/*map->playerElemnts[index] = 0;*/
			if (this->ready)
			{
				this->ready = false;
			}
			else
			{
				this->ready = true;
			}
			

		}
		/*if (gGamepads[index]->get_button_pressed(Gamepad::B))
		{

			map->playerElemnts[index] = 1;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::X))
		{
			
			map->playerElemnts[index] = 2;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::Y))
		{
			
			map->playerElemnts[index] = 3;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::Rb))
		{
			
			map->playerElemnts[index] = 4;
		}*/
		
		if (gGamepads[index]->get_button_pressed(Gamepad::Up))
		{
			if (!this->ready)
			{
				if (map->playerElemnts[index] > 0)
				{
					map->playerElemnts[index]--;
				}
				else
					map->playerElemnts[index] = 4;
			}
		}else if (gGamepads[index]->get_button_pressed(Gamepad::Down))
		{
			if (!this->ready)
			{
				if (map->playerElemnts[index] < 4)
				{
					map->playerElemnts[index]++;
				}
				else
					map->playerElemnts[index] = 0;
			}
			
		}


		if (map->playerElemnts[index] == 0)
		{
			delete element;
			element = new ArcaneElement();
		}else if (map->playerElemnts[index] == 1)
		{
			delete element;
			element = new FireElement();
		}
		else if(map->playerElemnts[index] == 2)
		{
			delete element;
			element = new WindElement();
		}
		else if(map->playerElemnts[index] == 3)
		{
			delete element;
			element = new EarthElement();
		}
		else if(map->playerElemnts[index] == 4)
		{
			delete element;
			element = new WaterElement();
		}

		if (gGamepads[index]->get_button_pressed(Gamepad::Start))
		{
			this->ready = true;
		}

	}

	if (*map->currentState == GameState::EndRound)
	{
		if (pUpgrades[index].choice[gMapConstants.round - 1] == 0)
		{
			pUpgrades[index].chooseUpgrade(1);
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::X))
		{
			pUpgrades[index].chooseUpgrade(1);
			map->upgradeChoice[index] = 0;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::Y))
		{
			pUpgrades[index].chooseUpgrade(2);
			map->upgradeChoice[index] = 1;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::A))
		{
			pUpgrades[index].chooseUpgrade(3);
			map->upgradeChoice[index] = 2;
		}
		if (gGamepads[index]->get_button_pressed(Gamepad::B))
		{
			pUpgrades[index].chooseUpgrade(4);
			map->upgradeChoice[index] = 3;
		}

		if (gGamepads[index]->get_button_pressed(Gamepad::Up))
		{
			if (map->upgradeChoice[index] > 0)
			{
				map->upgradeChoice[index]--;
			}
			else
				map->upgradeChoice[index] = 3;
		}
		else if (gGamepads[index]->get_button_pressed(Gamepad::Down))
		{
			if (map->upgradeChoice[index] < 3)
			{
				map->upgradeChoice[index]++;
			}
			else
				map->upgradeChoice[index] = 0;
		}

		if (gGamepads[index]->get_button_pressed(Gamepad::Start))
		{
			this->ready = true;
		}
	}

}
