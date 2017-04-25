#include "SoundSystem.h"

SoundSystem::SoundSystem()
{
	this->buffers = new sf::SoundBuffer[21];
	this->buffers[1].loadFromFile("../Resources/Sounds/boom.wav");



	this->music.openFromFile("../Resources/Sounds/BGM.wav");
}

SoundSystem::~SoundSystem()
{
	delete [] buffers;
}

void SoundSystem::play(spellSounds s)
{
	
	this->sound.setBuffer(this->buffers[s]);
	this->sound.play();
}

void SoundSystem::startBGM()
{
	this->music.play();
}
