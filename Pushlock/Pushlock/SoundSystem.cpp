#include "SoundSystem.h"

SoundSystem::SoundSystem()
{
	this->buffers = new sf::SoundBuffer[21];
	this->sound = new sf::Sound[32];
	this->buffers[0].loadFromFile("../Resources/Sounds/fireball.wav");
	this->buffers[1].loadFromFile("../Resources/Sounds/boom.wav");



	this->music.openFromFile("../Resources/Sounds/BGM.wav");
}

SoundSystem::~SoundSystem()
{
	delete [] buffers;
}

void SoundSystem::play(spellSounds s, float offset)
{
	int i = 0;
	bool cont = true;
	sf::Time t = sf::seconds(offset);
	while (cont)
	{
		if (this->sound[i].getStatus() == sf::Sound::Status::Playing)
		{
			i++;
		}
		else
		{
			cont = false;
		}
	}
	

	this->sound[i].setPlayingOffset(t);
	this->sound[i].setBuffer(this->buffers[s]);
	this->sound[i].play();
}

void SoundSystem::startBGM()
{
	this->music.setVolume(0);
	this->music.play();
}
