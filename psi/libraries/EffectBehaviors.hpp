#pragma once

#include "Settings.hpp"
#include "Definitions.hpp"

class IEffectBehavior
{
public:
	virtual void Execute(Target& target) = 0;
	virtual ~IEffectBehavior() = default;
};

// Cards
class BuffBehavior : public IEffectBehavior
{
	StatType statType;
	int value;
public:
	BuffBehavior(StatType statType, int value) : statType(statType), value(value) {}
	void Execute(Target& target) override;
};

// Cards
class DebuffBehavior : public IEffectBehavior
{
	StatType statType;
	int value;
public:
	DebuffBehavior(StatType statType, int value) : statType(statType), value(value) {}
	void Execute(Target& target) override;
};

// Unit on the battlefield and Hero
class DamageBehavior : public IEffectBehavior 
{
	int damageAmount;
public:
	DamageBehavior(int value) : damageAmount(value) {}
	void Execute(Target& target) override;
};

// Unit on the battlefield and Hero
class HealBehavior : public IEffectBehavior
{
	int healAmount;
public:
	HealBehavior(int value) : healAmount(value) {}
	void Execute(Target& target) override;
};

// Unit on the battlefield
class StatusApplyBehavior : public IEffectBehavior
{
public:
	StatusApplyBehavior() {}
	void Execute(Target& target) override;
};

// Unit on the battlefield
class StatusRemoveBehavior : public IEffectBehavior
{
public:
	StatusRemoveBehavior() {}
	void Execute(Target& target) override;
};

// Unit on the battlefield
class KeywordAddBehavior : public IEffectBehavior
{
public:
	KeywordAddBehavior() {}
	void Execute(Target& target) override;
};

class SummonBehavior : public IEffectBehavior
{
public:
	SummonBehavior() {}
	void Execute(Target& target) override;
};

// Cards from deck
class DrawBehavior : public IEffectBehavior
{
public:
	DrawBehavior() {}
	void Execute(Target& target) override;
};

// Cards from hand
class DiscardBehavior : public IEffectBehavior
{
public:
	DiscardBehavior() {}
	void Execute(Target& target) override;
};

// Hero
class EnergyModifyBehavior : public IEffectBehavior
{
public:
	EnergyModifyBehavior() {}
	void Execute(Target& target) override;
};

// Unit on the battlefield
class SilenceBehavior : public IEffectBehavior
{
public:
	SilenceBehavior() {}
	void Execute(Target& target) override;
};

// Deck and hand
class ShuffleBehavior : public IEffectBehavior
{
public:
	ShuffleBehavior() {}
	void Execute(Target& target) override;
};

// Cards from hand or deck
class StealBehavior : public IEffectBehavior
{
public:
	StealBehavior() {}
	void Execute(Target& target) override;
};