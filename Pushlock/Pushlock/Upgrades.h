#pragma once

struct Upgrades {
	int choice[3] = {0, 0, 0};

	void chooseUpgrade(int round, int cchoice)
	{
		choice[round - 1] = cchoice;
	}
};

extern Upgrades pUpgrades[4];