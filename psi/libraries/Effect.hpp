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
	void setSummonBehavior();
	void setCastBehavior();
	void setEquipBehavior();
	void setDrawBehavior();
	void setDiscardBehavior();
	void setShuffleBehavior();
	void setStealBehavior();
	void setEnergyModifyBehavior();
public:
	Effect(const uint_least32_t& effectSeed, const CardType cardType);
};