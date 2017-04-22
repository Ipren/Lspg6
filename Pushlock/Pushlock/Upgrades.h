#pragma once

struct Upgrades {
	int choice[3] = {0, 0, 0};
	int round = 0;
	void chooseUpgrade(int cchoice)
	{
		choice[round] = cchoice;
	}
};

extern Upgrades pUpgrades[4];