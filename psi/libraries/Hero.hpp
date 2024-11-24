#pragma once

#include <algorithm>

#include "Settings.hpp"
#include "Card.hpp"

class Hero : public Target
{
private:
	int maxHealth = 25;				// Max value of health
	int currentHealth;				// Current health that is updateg constantly

	int maxEnergy;					// Depending on turns
	int currentEnergy;				// Current energy stored in turn

	std::vector<Card*> deck;		// Deck that player has
	std::vector<Card*> hand;		// Hand that player has during turns
	std::vector<Card*> battlefield;	// Battlefied corresponding to player
public:
	void dealDamage(int value);
	void restoreHealth(int value);
};

inline void Hero::dealDamage(int value)
{

}

inline void Hero::restoreHealth(int value)
{
	currentHealth += value;
	currentHealth = std::min(currentHealth, maxHealth);
}
