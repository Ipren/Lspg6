#pragma once

struct Upgrades {
	int choice[3] = {1, 1, 1};
	int round = 0;
	void chooseUpgrade(int cchoice)
	{
		choice[round] = cchoice;
	}
};

extern Upgrades pUpgrades[4];