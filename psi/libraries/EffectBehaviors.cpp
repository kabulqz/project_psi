#include "EffectBehaviors.hpp"
#include "Card.hpp"
#include "Hero.hpp"

void BuffBehavior::Execute(Target& target)
{
	// Apply buff to target
	Card* card = dynamic_cast<Card*>(&target);
	if (!card) return;

	if (statType == StatType::ENERGY_COST)
	{
		if (card->getZone() == TargetZone::HAND || card->getZone() == TargetZone::DECK)
		{
			card->reduceEnergyCost(value);
		}
	}

	if (ItemCard* item = dynamic_cast<ItemCard*>(card))
	{
		if (statType == StatType::DAMAGE)
		{
			item->increaseDamage(value);
		}
		else if (statType == StatType::DEFENSE)
		{
			item->increaseDefense(value);
		}
	}
	else if (UnitCard* unit = dynamic_cast<UnitCard*>(card))
	{
		if (statType == StatType::HEALTH)
		{
			unit->increaseHealth(value);
		}
		else if (statType == StatType::ATTACK)
		{
			unit->increaseAttack(value);
		}
	}
}

void DebuffBehavior::Execute(Target& target)
{
	// Apply debuff to target
	Card* card = dynamic_cast<Card*>(&target);
	if (!card) return;

	if (statType == StatType::ENERGY_COST)
	{
		if (card->getZone() == TargetZone::HAND || card->getZone() == TargetZone::DECK)
		{
			card->increaseEnergyCost(value);
		}
	}

	if (ItemCard* item = dynamic_cast<ItemCard*>(card))
	{
		if (statType == StatType::DAMAGE)
		{
			item->decreaseDamage(value);
		}
		else if (statType == StatType::DEFENSE)
		{
			item->decreaseDefense(value);
		}
	}
	else if (UnitCard* unit = dynamic_cast<UnitCard*>(card))
	{
		if (statType == StatType::HEALTH)
		{
			unit->decreaseHealth(value);
		}
		else if (statType == StatType::ATTACK)
		{
			unit->decreaseAttack(value);
		}
	}
}

void DamageBehavior::Execute(Target& target)
{
	if (Hero* hero = dynamic_cast<Hero*>(&target))
	{
		hero->dealDamage(damageAmount);
		return;
	}
	else if (UnitCard* unit = dynamic_cast<UnitCard*>(&target))
	{
		unit->dealDamage(damageAmount);
		return;
	}
}

void HealBehavior::Execute(Target& target)
{
	if (Hero* hero = dynamic_cast<Hero*>(&target))
	{
		hero->restoreHealth(healAmount);
		return;
	}
	else if (UnitCard* unit = dynamic_cast<UnitCard*>(&target))
	{
		unit->restoreHealth(healAmount);
		return;
	}
}

void StatusApplyBehavior::Execute(Target& target)
{
}

void StatusRemoveBehavior::Execute(Target& target)
{
}

void KeywordAddBehavior::Execute(Target& target)
{
}

void SummonBehavior::Execute(Target& target)
{
}

void DrawBehavior::Execute(Target& target)
{
}

void DiscardBehavior::Execute(Target& target)
{
}

void EnergyModifyBehavior::Execute(Target& target)
{
}

void SilenceBehavior::Execute(Target& target)
{
}

void ShuffleBehavior::Execute(Target& target)
{
}

void StealBehavior::Execute(Target& target)
{
}