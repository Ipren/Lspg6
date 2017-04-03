#pragma once
#include "Definitions.h"
#include <Windows.h>
#include <Xinput.h>
#include <DirectXMath.h>

using namespace DirectX;

class Gamepad
{
public:
	


	Gamepad(unsigned int index);
	~Gamepad();

	void update(float dt);

	XMFLOAT2 get_left_thumb() const;
	XMFLOAT2 get_right_thumb() const;

	float get_left_thumb_angle() const;
	float get_right_thumb_angle() const;

	bool get_button_down(Button butt);// const;
	bool get_button_pressed(Button butt);//const;

	//input rumble: 0-1
	//input motor: 0-both, 1-left, 2-right
	void set_rumble(float rumble, int motor = 0);

private:
	bool buttons[16];//size = nrOfButtons
	unsigned int index;

	XMFLOAT2 left_thumb;
	XMFLOAT2 right_thumb;
	
	float left_angle;
	float right_angle;

	bool connected;
	// håll reda på hur länge vi har varit disconnectad för att kunna kolla efter
	// reconnect varje n sekunder
	float reconn_time;
};

