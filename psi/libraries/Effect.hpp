#pragma once

#include "EffectBehaviors.hpp"

class Effect
{
	std::mt19937 generator;

	EffectCategory category;
	EffectTrigger trigger;
	EffectDuration durationType;
	TargetMode targetMode;
	TargetZone targetZone;
	TargetGroup targetGroup;

	std::unique_ptr<IEffectBehavior> behavior;
	CardType cardType;

	void setBuffBehavior();
	void setDebuffBehavior();
	void setHealBehavior();
	void setDamageBehavior();
	void setStatusApplyBehavior();
	void setKeywordAddBehavior();
	void setSilenceBehavior();
	void setStatusRemoveBehavior();
	void setDrawBehavior();
	void setDiscardBehavior();
	void setShuffleBehavior();
	void setStealBehavior();
	void setEnergyModifyBehavior();
public:
	Effect(const uint_least32_t& effectSeed, const CardType cardType);

	// Execute effect, card - card that initiated the effect
	// trigger - which trigger activated the effect
	// event - optional event that triggered the effect, used for ON_GAME_EVENT trigger
	void executeEffect(Card* card, EffectTrigger trigger, std::optional<GameEvent> event);
};