#include "Gamepad.h"

#include <math.h>

#include "Globals.h"

// saxat från https://msdn.microsoft.com/en-us/library/windows/desktop/ee417001(v=vs.85).aspx
static XMFLOAT2 normalize_thumbs(float x, float y, float deadzone)
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

	return {
		normalized_x * normalized_magnitude,
		normalized_y * normalized_magnitude
	};
}

Gamepad::Gamepad(unsigned int index) :
	index(index), reconn_time(0.f), left_thumb(0.f, 0.f), right_thumb(0.f, 0.f),
	left_angle(0.f), right_angle(0.f)
{
	XINPUT_STATE state;
	connected = XInputGetState(index, &state) == ERROR_SUCCESS;
}

Gamepad::~Gamepad()
{
}

XMFLOAT2 Gamepad::get_left_thumb() const
{
	return left_thumb;
}

XMFLOAT2 Gamepad::get_right_thumb() const
{
	return right_thumb;
}

float Gamepad::get_left_thumb_angle() const
{
	return left_angle;
}

float Gamepad::get_right_thumb_angle() const
{
	return right_angle;
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
		left_thumb = normalize_thumbs(
			state.Gamepad.sThumbLX,
			state.Gamepad.sThumbLY,
			XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
		);

		right_thumb = normalize_thumbs(
			state.Gamepad.sThumbRX,
			state.Gamepad.sThumbRY,
			XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
		);

		if (left_thumb.x != 0 || left_thumb.y != 0) {
			left_angle = XM_PI * 0.5 - atan2f(left_thumb.y, left_thumb.x);
		}

		if (right_thumb.x != 0 || right_thumb.y != 0) {
			right_angle = XM_PI * 0.5 - atan2f(right_thumb.y, right_thumb.x);
		}
	}
	else {
		connected = false;

		left_thumb = { 0.f, 0.f };
		right_thumb = { 0.f, 0.f };
	}
}
