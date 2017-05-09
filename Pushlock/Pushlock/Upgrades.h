#pragma once
#include "Constants.h"
struct Upgrades {
	int choice[3] = {0, 0, 0};
	void chooseUpgrade(int cchoice)
	{
		choice[gMapConstants.round - 1] = cchoice;
	}
	void resetUpgrades(){
		for (size_t i = 0; i < 3; i++)
		{
			choice[i] = 0;
		}
		for (size_t i = 0; i < 4; i++)
		{
			gPlayerSpellConstants[i].kHealth = 0.f;
		}
	}
};

extern Upgrades pUpgrades[4];