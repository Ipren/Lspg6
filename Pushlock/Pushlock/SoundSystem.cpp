#include "SoundSystem.h"
#include <tchar.h>
#include <windows.h>
#include "mmsystem.h"

#pragma comment(lib, "winmm.lib")


SoundSystem::SoundSystem()
{
	this->buffers = new sf::SoundBuffer[21];
	this->sound = new sf::Sound[32];
	this->buffers[0].loadFromFile("../Resources/Sounds/fireball.wav");
	this->buffers[1].loadFromFile("../Resources/Sounds/boom.wav");
	this->buffers[2].loadFromFile("../Resources/Sounds/arcaneball.wav");
	this->buffers[3].loadFromFile("../Resources/Sounds/earthball.wav");
	this->buffers[5].loadFromFile("../Resources/Sounds/iceball.wav");


	this->buffers[7].loadFromFile("../Resources/Sounds/arcanestomp.wav");



	this->buffers[14].loadFromFile("../Resources/Sounds/winddash.flac");

	this->buffers[18].loadFromFile("../Resources/Sounds/earthwall.wav");

}

SoundSystem::~SoundSystem()
{
	delete [] buffers;
}

void SoundSystem::play(spellSounds s, float offset, float volume)
{
	if (waveOutGetNumDevs()) 
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

		this->sound[i].setVolume(volume);
		this->sound[i].setPlayingOffset(t);
		this->sound[i].setBuffer(this->buffers[s]);
		this->sound[i].play();
	}
	
}

void SoundSystem::startBGM()
{
	this->music.setVolume(0);
	this->music.play();
}
