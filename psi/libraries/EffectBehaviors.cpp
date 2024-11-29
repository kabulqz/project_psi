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

void CastBehavior::Execute(Target& target)
{
}

void EquipBehavior::Execute(Target& target)
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
	if (Card* card = dynamic_cast<Card*>(&target))
	{
		Hero* hero = nullptr;
		// Determine which player's deck to shuffle into

		if (card->getZone() == TargetZone::HAND || card->getZone() == TargetZone::BATTLEFIELD)
		{
			// If the cardis from your hand or battlefield, use the hero associated with the card
			if (Hero* cardOwner = dynamic_cast<Hero*>(card->getOwner()))
			{
				hero = cardOwner; // Get the owner of the card
			}
		}

		if (hero)
		{
			// Remove the card from its current zone (hand or battlefield)
			if (card->getZone() == TargetZone::HAND)
			{
				auto it = std::find(hero->getHand().begin(), hero->getHand().end(), card);
				if (it != hero->getHand().end())
				{
					hero->getHand().erase(it);
				}
			}
			else if (card->getZone() == TargetZone::BATTLEFIELD)
			{
				auto it = std::find(hero->getBattlefield().begin(), hero->getBattlefield().end(), card);
				if (it != hero->getBattlefield().end() && card->getIsOnBoard() == true)
				{
					hero->getBattlefield().erase(it);
				}
			}
		}

		hero->shuffleCardIntoTheDeck(card);
		card->setZone(TargetZone::DECK);
		card->setIsOnBoard(false);
	}
}

void StealBehavior::Execute(Target& target)
{
}