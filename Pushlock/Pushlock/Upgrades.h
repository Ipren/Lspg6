#pragma once

struct Upgrades {
	int choice[3] = {0, 0, 0};
	int round = 0;
	void chooseUpgrade(int cchoice)
	{
		choice[round] = cchoice;
	}
	//doesn't work
	void resetUpgrades(){
		for (size_t i = 0; i < 3; i++)
		{
			choice[i] = 0;
		}
		for (size_t i = 0; i < 4; i++)
		{
			gPlayerSpellConstants[i].kHealth = 0.f;
		}
		round = 0;
	}
};

extern Upgrades pUpgrades[4];