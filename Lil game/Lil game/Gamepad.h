#pragma once

#include <Windows.h>
#include <Xinput.h>

class Gamepad
{
public:
	Gamepad(unsigned int index);
	~Gamepad();

	void update(float dt);

	float get_left_thumb_x();
	float get_left_thumb_y();

	float get_right_thumb_x();
	float get_right_thumb_y();

	//input rumble: 0-1
	//input motor: 0-both, 1-left, 2-right
	void set_rumble(float rumble, int motor = 0);

private:
	unsigned int index;

	float left_thumb_x;
	float left_thumb_y;

	float right_thumb_x;
	float right_thumb_y;

	bool connected;
	// håll reda på hur länge vi har varit disconnectad för att kunna kolla efter
	// reconnect varje n sekunder
	float reconn_time;
};

