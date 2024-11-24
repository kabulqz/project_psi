#pragma once

#include "Settings.hpp"
#include "Definitions.hpp"

class Hero : public Target
{
private:
	int maxHealth = 25;				// Max value of health
	int currentHealth;				// Current health that is updateg constantly

	int maxEnergy;					// Depending on turns
	int currentEnergy;				// Current energy stored in turn

	std::stack<Card*>& deck;		// Deck that player has
	std::vector<Card*>& hand;		// Hand that player has during turns
	std::vector<Card*>& battlefield;// Battlefied corresponding to player
	int fatigue = 1;				// Fatigue that player will draw when there are no cards in deck
public:
	Hero();
	std::stack<Card*>& getDeck() const;
	std::vector<Card*>& getHand() const;
	std::vector<Card*>& getBattlefield() const;

	void dealDamage(int value);
	void restoreHealth(int value);
	void drawCard();
	void shuffleCardIntoTheDeck(Card* card) const;
};