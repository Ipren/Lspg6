#pragma once

struct Upgrades {
	int arcanechoice[] = {0, 0, 0};

	int firechoice[] = { 0, 0, 0 };

	int windchoice[] = { 0, 0, 0 };

	int earthchoice[] = { 0, 0, 0 };

	int waterchoice[] = { 0, 0, 0 };

	void chooseUpgrade(int round, int element, int choice)
	{
		if (element == 1)
		{
			arcanechoice[round-1] = choice
		}
		else if (element == 2)
		{
			firechoice[round - 1] = choice
		}
		else if (element == 3)
		{
			windchoice[round - 1] = choice
		}
		else if (element == 4)
		{
			earthchoice[round - 1] = choice
		}
		else if (element == 5)
		{
			waterchoice[round - 1] = choice
		}
	}
};