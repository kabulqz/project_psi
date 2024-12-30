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
	Card* effectTarget = nullptr;

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
	void decrementTurn() override;
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
	void decrementTurn() override;
	void checkForEndEvent(GameEvent event) override;
	void removeDebuff();

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