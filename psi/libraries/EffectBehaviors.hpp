#pragma once

#include "Settings.hpp"
#include "Definitions.hpp"

class IEffectBehavior
{
protected:
	// must include all the possible triggers and durations for the effect
	std::optional<EffectTrigger> trigger;
	std::optional<EffectDuration> duration;

	//general value
	std::optional<int> value;

	//statType for buff/debuff
	std::optional<StatType> statType;

	// to track number of targets
	std::optional<int> numberOfTargets;

	//for duration: TURN_BASED
	std::optional<int> numberOfTurns;

	// If trigger is ON_GAME_EVENT
	std::optional<GameEvent> triggerEvent;
	// If duratiion is EVENT_BASED
	std::optional<GameEvent> endEvent;

	// If category of the effect is STATUS_APPLY, STATUS_REMOVE
	std::optional<Status> status;
	// If category of the effect is KEYWORD_ADD
	std::optional<Keyword> keyword;
public:
	// Tracking the owner of the effect
	Target* effectTarget = nullptr;

	IEffectBehavior() = default;
	virtual ~IEffectBehavior() = default;

	virtual void execute(Target& target) = 0;
	virtual void decrementTurn() = 0;
	virtual void checkForEndEvent(GameEvent event) = 0;
};

class BuffBehavior : public IEffectBehavior
{
	std::optional<uint_least32_t> securityKey;
public:
	void execute(Target& target) override;
	void decrementTurn() override;						// If duration is TURN_BASED
	void checkForEndEvent(GameEvent event) override;
	void removeBuff() const;

	// BuffBehavior for Effect class
	BuffBehavior(
		EffectTrigger trigger,
		EffectDuration duration,
		int value,
		StatType statType,
		int numberOfTargets,
		std::optional<int> numberOfTurns = std::nullopt,
		std::optional<GameEvent> triggerEvent = std::nullopt,
		std::optional<GameEvent> endEvent = std::nullopt
	);

	// BuffBehavior for Card class
	BuffBehavior(
		Card* targetOfEffect,
		uint_least32_t securityKey,
		EffectDuration duration,
		int value,
		StatType statType,
		std::optional<int> numberOfTurns = std::nullopt,
		std::optional<GameEvent> endEvent = std::nullopt
	);
};

class DebuffBehavior : public IEffectBehavior
{
	std::optional<uint_least32_t> securityKey;
public:
	void execute(Target& target) override;
	void decrementTurn() override;						// If duration is TURN_BASED
	void checkForEndEvent(GameEvent event) override;
	void removeDebuff() const;

	// DebuffBehavior for Effect class
	DebuffBehavior(
		EffectTrigger trigger,
		EffectDuration duration,
		int value,
		StatType statType,
		int numberOfTargets,
		std::optional<int> numberOfTurns = std::nullopt,
		std::optional<GameEvent> triggerEvent = std::nullopt,
		std::optional<GameEvent> endEvent = std::nullopt
	);

	// DebuffBehavior for Card class
	DebuffBehavior(
		Card* targetOfEffect,
		uint_least32_t securityKey,
		EffectDuration duration,
		int value,
		StatType statType,
		std::optional<int> numberOfTurns = std::nullopt,
		std::optional<GameEvent> endEvent = std::nullopt
	);
};

class HealBehavior : public IEffectBehavior
{
	std::optional<uint_least32_t> securityKey;
public:
	void execute(Target& target) override;				// INSTANT - heal target, TURN_BASED - add lasting heal effect
	void decrementTurn() override;						// If duration is TURN_BASED
	void checkForEndEvent(GameEvent event) override;	// Not used

	// HealBehavior for Effect class
	HealBehavior(
		EffectTrigger trigger,
		EffectDuration duration,
		int value,
		int numberOfTargets,
		std::optional<int> numberOfTurns = std::nullopt,
		std::optional<GameEvent> triggerEvent = std::nullopt
	);

	// HealBehavior for Card/Hero class
	HealBehavior(
		Target* targetOfEffect,
		uint_least32_t securityKey,
		EffectDuration duration,
		int value,
		std::optional<int> numberOfTurns = std::nullopt
	);
};

class DamageBehavior : public IEffectBehavior
{
	std::optional<uint_least32_t> securityKey;
public:
	void execute(Target& target) override;				// INSTANT - deal damage, TURN_BASED - add lasting damage effect
	void decrementTurn() override;						// If duration is TURN_BASED
	void checkForEndEvent(GameEvent event) override;	// Not used

	// DamageBehavior for Effect class
	DamageBehavior(
		EffectTrigger trigger,
		EffectDuration duration,
		int value,
		int numberOfTargets,
		std::optional<int> numberOfTurns = std::nullopt,
		std::optional<GameEvent> triggerEvent = std::nullopt
	);

	// DamageBehavior for Card/Hero class
	DamageBehavior(
		Target* targetOfEffect,
		uint_least32_t securityKey,
		EffectDuration duration,
		int value,
		std::optional<int> numberOfTurns = std::nullopt
	);
};