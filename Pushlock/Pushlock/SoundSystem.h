#pragma once
#include <SFML\Audio.hpp>
#include "Definitions.h"


class SoundSystem
{
public:
	SoundSystem();
	~SoundSystem();

	void play(spellSounds s);
	void startBGM();

private:
	sf::SoundBuffer *buffers;
	sf::Sound sound;
	sf::Music music;
};
