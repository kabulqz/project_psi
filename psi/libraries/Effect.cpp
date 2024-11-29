#include "Effect.hpp"

#include "Card.hpp"
#include "Hero.hpp"

void Effect::setBehavior(EffectCategory category)
{
	switch (category)
	{
	case EffectCategory::BUFF:
		behavior = std::make_unique<BuffBehavior>(statType, value);
		break;
	case EffectCategory::DEBUFF:
		behavior = std::make_unique<DebuffBehavior>(statType, value);
		break;
	case EffectCategory::DAMAGE:
		behavior = std::make_unique<DamageBehavior>(value);
		break;
	case EffectCategory::HEAL:
		behavior = std::make_unique<HealBehavior>(value);
		break;
	case EffectCategory::STATUS_APPLY:
		behavior = std::make_unique<StatusApplyBehavior>();
		break;
	case EffectCategory::STATUS_REMOVE:
		behavior = std::make_unique<StatusRemoveBehavior>();
		break;
	case EffectCategory::KEYWORD_ADD:
		behavior = std::make_unique<KeywordAddBehavior>();
		break;
	case EffectCategory::SUMMON:
		behavior = std::make_unique<SummonBehavior>();
		break;
	case EffectCategory::CAST:
		behavior = std::make_unique<CastBehavior>();
		break;
	case EffectCategory::EQUIP:
		behavior = std::make_unique<EquipBehavior>();
		break;
	case EffectCategory::DRAW:
		behavior = std::make_unique<DrawBehavior>();
		break;
	case EffectCategory::DISCARD:
		behavior = std::make_unique<DiscardBehavior>();
		break;
	case EffectCategory::ENERGY_MODIFY:
		behavior = std::make_unique<EnergyModifyBehavior>();
		break;
	case EffectCategory::SILENCE:
		behavior = std::make_unique<SilenceBehavior>();
		break;
	case EffectCategory::SHUFFLE:
		behavior = std::make_unique<ShuffleBehavior>();
		break;
	case EffectCategory::STEAL:
		behavior = std::make_unique<StealBehavior>();
		break;
	}
}

Effect::Effect()
{

	setBehavior(category);
}

Effect::Effect(EffectTrigger trigger, EffectCategory category, TargetMode targetMode, TargetGroup targetGroup, TargetZone targetZone, int value, EffectDuration durationType, int numberOfTurns, bool isActive)
{
	this->trigger = trigger;
	this->category = category;
	this->targetMode = targetMode;
	this->targetGroup = targetGroup;
	this->targetZone = targetZone;
	this->value = value;
	this->durationType = durationType;
	this->numberOfTurns = numberOfTurns;
	this->isActive = isActive;
	setBehavior(this->category);
}

void Effect::applyEffect(Target& target)
{
	if (!isActive) return;
	bool isValid = false;
	
	if (Card* card = dynamic_cast<Card*>(&target))
	{
		if (card->getZone() != targetZone) isValid = true;
	}
	else if (Hero* hero = dynamic_cast<Hero*>(&target))
	{
		isValid = true;
	}

	if (isValid && behavior)
	{
		behavior->Execute(target);

		if (durationType == EffectDuration::TURN_BASED)
		{
			--numberOfTurns;
			if (numberOfTurns <= 0) isActive = false;
			// If numberOfTurns is 0, go back to normal values
		}
		else if (durationType == EffectDuration::OVER_TIME)
		{
			// Apply incremental logic (e.g. re-execute effect every turn)
			--numberOfTurns;
			if (numberOfTurns <= 0) isActive = false;
		}
		else if (durationType == EffectDuration::EVENT_BASED)
		{
			// tzw efekt dziala do momentu wystapienia jakiegos eventu

			// Check if the event has occurred
			// If the event has occurred, set isActive to false
		}
	}
}

EffectTrigger Effect::getTrigger() const
{
	return trigger;
}

GameEvent Effect::getGameEventTrigger() const
{
	return gameEvent;
}