#pragma once

#include "Settings.hpp"
#include "EffectBehaviors.hpp"

class Effect
{
private:
	int value;									// Numerical value of the effect
	EffectCategory category;					// Category of the effect
	EffectTrigger trigger;						// When the effect is triggered
	EffectDuration durationType;				// Effect duration type
	int numberOfTurns;							// Number of turns left (for TURN_BASED)

	TargetMode targetMode;						// Mode of the target
	TargetZone targetZone;						// Zone of the target
	TargetGroup targetGroup;					// Group of the target
	bool isActive;								// Whether the effect is still active
	std::unique_ptr<IEffectBehavior> behavior;	// Behavior of the effect
	void setBehavior(EffectCategory category);	// Sets the behavior of the effect

	std::string description;					// Description of the effect

	//If chosen ON_GAME_EVENT, the following attribute is needed
	GameEvent gameEvent;						// Game event that triggers the effect, only with ON_GAME_EVENT trigger

	// If chosen BUFF or DEBUFF, the following attributes are needed
	StatType statType;							// Type of stat to buff/debuff

public:
	// Default constructor, generates new effect
	Effect();
	// Constructor with parameters, generates effect based on parameters (reading from save)
	Effect(EffectTrigger trigger, EffectCategory category, TargetMode targetMode, TargetGroup targetGroup, TargetZone targetZone, int value, EffectDuration durationType, int numberOfTurns, bool isActive);
	~Effect() = default;

	void applyEffect(Target& target);			// Applies the effect to the target
	EffectTrigger getTrigger() const;			// Returns the trigger of the effect
	GameEvent getGameEventTrigger() const;		// Returns the game event of the effect
};