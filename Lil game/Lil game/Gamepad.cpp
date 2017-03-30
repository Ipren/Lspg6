#include "Gamepad.h"

#include <math.h>

#include "Globals.h"

// saxat från https://msdn.microsoft.com/en-us/library/windows/desktop/ee417001(v=vs.85).aspx
static void normalize_thumbs(float x, float y, float deadzone, float *out_x, float *out_y)
{
	float magnitude = sqrt(x * x + y * y);

	float normalized_x = x / magnitude;
	float normalized_y = y / magnitude;
	float normalized_magnitude = 0;

	if (magnitude > deadzone)
	{
		if (magnitude > 32767) magnitude = 32767;

		magnitude -= deadzone;

		normalized_magnitude = magnitude / (32767 - deadzone);
	}
	else
	{
		magnitude = 0.0;
		normalized_magnitude = 0.0;
	}

	*out_x = normalized_x * normalized_magnitude;
	*out_y = normalized_y * normalized_magnitude;
}

Gamepad::Gamepad(unsigned int index) :
	index(index), reconn_time(0.f), left_thumb_x(0.f), left_thumb_y(0.f),
	right_thumb_x(0.f), right_thumb_y(0.f)
{
	connected = XInputGetState(index, nullptr) == ERROR_SUCCESS;
}

Gamepad::~Gamepad()
{
}

float Gamepad::get_left_thumb_x()
{
	return left_thumb_x;
}

float Gamepad::get_left_thumb_y()
{
	return left_thumb_y;
}

float Gamepad::get_left_thumb_angle()
{
	return atan2f(left_thumb_y, left_thumb_x);
}

float Gamepad::get_right_thumb_x()
{
	return right_thumb_x;
}

float Gamepad::get_right_thumb_y()
{
	return right_thumb_y;
}

float Gamepad::get_right_thumb_angle()
{
	return atan2f(right_thumb_y, right_thumb_x);
}

void Gamepad::set_rumble(float rumble, int motor)
{
	XINPUT_VIBRATION vib;
	ZeroMemory(&vib, sizeof(vib));
	if (motor == 1 || motor == 0)
	{
		vib.wLeftMotorSpeed = rumble * 65535;
	}
	if (motor == 2 || motor == 0)
	{
		vib.wRightMotorSpeed = rumble * 65535;
	}
	XInputSetState(this->index, &vib);
}

void Gamepad::update(float dt)
{
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	if (!connected) {
		reconn_time += dt;

		// efter RECONN_TIME sekunder kolla om vi har reconnectat igen
		if (reconn_time > RECONN_TIME) {
			reconn_time -= RECONN_TIME;
		
			connected = XInputGetState(index, &state) == ERROR_SUCCESS;
			if (!connected) return;
		}
		else {
			return;
		}
	}

	if (XInputGetState(index, &state) == ERROR_SUCCESS) {
		normalize_thumbs(
			state.Gamepad.sThumbLX,
			state.Gamepad.sThumbLY,
			XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
			&left_thumb_x,
			&left_thumb_y
		);

		normalize_thumbs(
			state.Gamepad.sThumbRX,
			state.Gamepad.sThumbRY,
			XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
			&right_thumb_x,
			&right_thumb_y
		);
	}
	else {
		connected = false;

		left_thumb_x = 0.f;
		left_thumb_y = 0.f;

		right_thumb_x = 0.f;
		right_thumb_y = 0.f;
	}
	
	

}
