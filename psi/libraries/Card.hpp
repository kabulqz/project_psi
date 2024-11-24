#pragma once

#include "Settings.hpp"
#include "Effect.hpp"
#include "Hero.hpp"

class Card : public Target
{
	//Visual elements of the card
	std::string back;
	std::string background;
	std::string portrait;
	std::string cardface;

	// Common attributes of the card
	TargetZone zone;									// Where the card is located
	int energyCost = 0;									// Mana cost of the card

	std::unordered_set<Keyword> keywords;				// List of keywords
	std::map<Status, int> statuses;						// List of statuses
	std::vector<std::shared_ptr<Effect>> effects;		// List of effects
	bool isOnBoard = false;								// Flag	to track if the card is on the board
	Hero& owner;											// Owner of the card
public:
	Card() = default;
	~Card() = default;
	void triggerEffects(const GameEvent& gameEvent, const EffectTrigger& effectTrigger) const;
	bool isEventTriggered(const GameEvent& gameEvent) const;
	TargetZone getZone() const;
	void setZone(const TargetZone& zone);
	bool getIsOnBoard() const;
	void setIsOnBoard(bool value);
	Hero* getOwner() const;
	void reduceEnergyCost(int value);
	void increaseEnergyCost(int value);
};

class UnitCard : public Card // If 0 health, card is destroyed
{
	int baseHealth;
	int currentHealth;
	int extraHealth = 0;
	int baseAttack;
	int currentAttack;
	int extraAttack = 0;
public:
	void restoreHealth(int value);
	void increaseHealth(int value);
	void dealDamage(int value);
	void decreaseHealth(int value);
	void increaseAttack(int value);
	void decreaseAttack(int value);

};

class ItemCard : public Card 
{
	int baseDamage;
	int currentDamage;
	int extraDamage = 0;
	int baseDefense;
	int currentDefense;
	int extraDefense = 0;
public:
	void increaseDamage(int value);
	void decreaseDamage(int value);
	void increaseDefense(int value);
	void decreaseDefense(int value);
};

class SpellCard : public Card
{
public:

};