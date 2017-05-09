#pragma once
#include "Constants.h"
struct Upgrades {
	int choice[3] = {0, 0, 0};
	void chooseUpgrade(int cchoice)
	{
		choice[gMapConstants.round - 1] = cchoice;
	}
	//doesn't work
	void resetUpgrades(){
		for (size_t i = 0; i < 3; i++)
		{
			choice[i] = 0;
		}
	}
};

extern Upgrades pUpgrades[4];