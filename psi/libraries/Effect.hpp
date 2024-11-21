#pragma once

#include "Settings.hpp"
#include "Game.hpp"
#include "GameEvent.hpp"

enum class EffectCategory
{
	HEAL,		   // Restores health
	DAMAGE,		   // Deals damage
	BUFF,          // Increases stats or provides bonuses
	DEBUFF,        // Reduces stats or applies penalties
	DRAW,          // Draws cards
	DISCARD,       // Forces discard
	SUMMON,        // Summons units or items
	STATUS_APPLY,  // Applies a specific status (e.g. bleeding, poison)
	STATUS_REMOVE, // Removes a specific status
	MANA_MODIFY,   // Adjusts mana (e.g., add or drain mana)
	KEYWORD_ADD,   // Adds a keyword to a card
	SILENCE,       // Disables card effects, buffs, statuses, keywords
	SPECIAL        // Any unique or custom effect
};

enum class Stat
{
	HEALTH,
	ATTACK,
	MANA
};

enum class EffectType
{
	GLOBAL,
	TARGETED
};

enum class TargetType
{
	NONE,
	SELF,
	SINGLE,
	MULTIPLE,
	RANDOM_SINGLE,
	RANDOM_MULTIPLE,
};

class Card;

// Base effect class
class Effect
{
protected:
	std::string description;
	int value;
	EffectCategory effectCategory;
	EffectType effectType;
	TargetType targetType;
	bool isPermanent;
	int duration;
	GameEvent triggersOn;
	Stat statistic;
public:
	Effect(Game* game, int val, EffectCategory category, EffectType type, TargetType target, bool permanent = false, int duration = 1, GameEvent trigger = GameEvent::PLAYER_PLAY);
	~Effect() = default;

	// Apply effect to target
	virtual void apply(Card& target) = 0;

	// Check if effect applies under certain conditions
	bool isTriggered(const GameEvent& gameEvent) const;

	// Accessors
	int getValue() const;
	EffectCategory getCategory() const;
	EffectType getType() const;
	TargetType getTargetType() const;
	bool isPermanentEffect() const;
	int getDuration() const;

	// Effect description for debugging or display purposes
	std::string getDescription() const;

	// Decrease duration of temporary effect
	void decreaseDuration();
};