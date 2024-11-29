#include "Card.hpp"

void Card::triggerEffects(const GameEvent& gameEvent, const EffectTrigger& effectTrigger) const
{
	for (const auto& effect : effects)
	{
		if (effect->getTrigger() == effectTrigger)
		{
			if (effect->getTrigger() == EffectTrigger::IMMEDIATE)
			{
				
			}
			else if (effect->getTrigger() == EffectTrigger::ON_DRAW)
			{
				
			}
			else if (effect->getTrigger() == EffectTrigger::ON_GAME_EVENT)
			{
				if (isEventTriggered(gameEvent))
				{
					
				}
			}
		}
	}
}

bool Card::isEventTriggered(const GameEvent& gameEvent) const
{
	for (const auto& effect : effects)
	{
		if (effect->getGameEventTrigger() == gameEvent)
		{
			return true;
		}
	}
	return false;
}

TargetZone Card::getZone() const
{
	return zone;
}

void Card::setZone(const TargetZone& zone)
{
	this->zone = zone;
}

bool Card::getIsOnBoard() const
{
	return isOnBoard;
}

void Card::setIsOnBoard(bool value)
{
	isOnBoard = value;
}

Hero* Card::getOwner() const
{
	return &owner;
}

void Card::reduceEnergyCost(int value)
{
	energyCost -= value;
	energyCost = std::max(0, energyCost);
}

void Card::increaseEnergyCost(int value)
{
	energyCost += value;
}

void UnitCard::restoreHealth(int value)
{
	currentHealth += value;
	currentHealth = std::min(currentHealth, baseHealth + extraHealth);
}


void UnitCard::increaseHealth(int value)
{
	extraHealth += value;
}

void UnitCard::dealDamage(int value)
{
	currentHealth -= value;
	currentHealth = std::max(currentHealth, 0);
	if (currentHealth == 0)
	{
		// Destroy card
	}
}

void UnitCard::decreaseHealth(int value)
{
	currentHealth -= value;
	currentHealth = std::max(currentHealth, 0);
	if (currentHealth == 0)
	{
		// Destroy card
	}
}

void UnitCard::increaseAttack(int value)
{
	extraAttack += value;
}

void UnitCard::decreaseAttack(int value)
{
	currentAttack -= value;
	currentAttack = std::max(currentAttack, 0);
}

void ItemCard::increaseDamage(int value)
{
	extraDamage += value;
}

void ItemCard::decreaseDamage(int value)
{
	currentDamage -= value;
	currentDamage = std::max(currentDamage, 0);
}

void ItemCard::increaseDefense(int value)
{
	extraDefense += value;
}

void ItemCard::decreaseDefense(int value)
{
	currentDefense -= value;
	currentDefense = std::max(currentDefense, 0);
}
