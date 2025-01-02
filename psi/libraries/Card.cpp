#include "Card.hpp"

void Card::reduceEnergyCost(const int value, const uint_least32_t key)
{
	extraEnergyCost.push_back(std::make_unique<EffectValue>(-value, key, StatType::ENERGY_COST));
	currentEnergyCost = std::max(0, baseEnergyCost + std::accumulate(extraEnergyCost.begin(), extraEnergyCost.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void Card::increaseEnergyCost(const int value, const uint_least32_t key)
{
	extraEnergyCost.push_back(std::make_unique<EffectValue>(value, key, StatType::ENERGY_COST));
	currentEnergyCost = std::max(0, baseEnergyCost + std::accumulate(extraEnergyCost.begin(), extraEnergyCost.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void Card::applyEffect(std::unique_ptr<IEffectBehavior> effectBehavior)
{
	activeEffects.push_back(std::move(effectBehavior));
}

void Card::removeEffect(IEffectBehavior* effectBehavior)
{
	if (auto* buffBehavior = dynamic_cast<BuffBehavior*>(effectBehavior))
	{
		buffBehavior->removeBuff();
	}
	else if (auto* debuffBehavior = dynamic_cast<DebuffBehavior*>(effectBehavior))
	{
		debuffBehavior->removeDebuff();
	}

	// Remove the effect from the active effects list
	std::erase_if(activeEffects, [effectBehavior](const std::unique_ptr<IEffectBehavior>& effect)
		{
			return effect.get() == effectBehavior;
		});
}

void UnitCard::increaseHealth(const int value, const uint_least32_t key)
{
	extraHealth.push_back(std::make_unique<EffectValue>(value, key, StatType::HEALTH));
	currentHealth = std::min(currentHealth + value, std::accumulate(extraHealth.begin(), extraHealth.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void UnitCard::reduceHealth(const int value, const uint_least32_t key)
{
	extraHealth.push_back(std::make_unique<EffectValue>(-value, key, StatType::HEALTH));
	currentHealth = std::min(currentHealth, baseHealth + std::accumulate(extraHealth.begin(), extraHealth.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));

	if (currentHealth <= 0)
	{
		destroy();
	}
}

void UnitCard::increaseAttack(const int value, const uint_least32_t key)
{
	extraAttack.push_back(std::make_unique<EffectValue>(value, key, StatType::ATTACK));
	currentAttack = std::min(currentAttack + value, std::accumulate(extraAttack.begin(), extraAttack.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void UnitCard::reduceAttack(const int value, const uint_least32_t key)
{
	extraAttack.push_back(std::make_unique<EffectValue>(-value, key, StatType::ATTACK));
	currentAttack = std::min(currentAttack, baseAttack + std::accumulate(extraAttack.begin(), extraAttack.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void UnitCard::heal(const int value)
{
	int maxHealth = baseHealth + std::accumulate(extraHealth.begin(), extraHealth.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		});

	// Zwiêksz zdrowie o wartoœæ, ale nie wiêcej ni¿ maxHealth
	currentHealth = std::min(currentHealth + value, maxHealth);
}

void UnitCard::dealDamage(int value)
{
	// Zmniejsz zdrowie o wartoœæ, ale nie mniej ni¿ 0
	currentHealth = std::max(0, currentHealth - value);
	if (currentHealth <= 0)
	{
		destroy();
	}
}

void UnitCard::destroy()
{// Destroy the unit card

}

void ItemCard::increaseDamage(const int value, const uint_least32_t key)
{
	extraDamage.push_back(std::make_unique<EffectValue>(value, key, StatType::DAMAGE));
	currentDamage = std::min(currentDamage + value, std::accumulate(extraDamage.begin(), extraDamage.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void ItemCard::reduceDamage(const int value, const uint_least32_t key)
{
	extraDamage.push_back(std::make_unique<EffectValue>(-value, key, StatType::DAMAGE));
	currentDamage = std::min(currentDamage, baseDamage + std::accumulate(extraDamage.begin(), extraDamage.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void ItemCard::increaseDefense(const int value, const uint_least32_t key)
{
	extraDefense.push_back(std::make_unique<EffectValue>(value, key, StatType::DEFENSE));
	currentDefense = std::min(currentDefense + value, std::accumulate(extraDefense.begin(), extraDefense.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void ItemCard::reduceDefense(const int value, const uint_least32_t key)
{
	extraDefense.push_back(std::make_unique<EffectValue>(-value, key, StatType::DEFENSE));
	currentDefense = std::min(currentDefense, baseDefense + std::accumulate(extraDefense.begin(), extraDefense.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void ItemCard::increaseDurability(const int value, const uint_least32_t key)
{
	extraDurability.push_back(std::make_unique<EffectValue>(value, key, StatType::DURABILITY));
	currentDurability = std::min(currentDurability + value, std::accumulate(extraDurability.begin(), extraDurability.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void ItemCard::reduceDurability(const int value, const uint_least32_t key)
{
	extraDurability.push_back(std::make_unique<EffectValue>(-value, key, StatType::DURABILITY));
	currentDurability = std::min(currentDurability, baseDurability + std::accumulate(extraDurability.begin(), extraDurability.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));

	if (currentDurability <= 0)
	{
		destroy();
	}
}

void ItemCard::destroy()
{// Destroy the item card

}

void SpellCard::increaseValue(const int value, const uint_least32_t key)
{
	extraValue.push_back(std::make_unique<EffectValue>(value, key, StatType::VALUE));
	currentValue = std::min(currentValue + value, std::accumulate(extraValue.begin(), extraValue.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}

void SpellCard::reduceValue(const int value, const uint_least32_t key)
{
	extraValue.push_back(std::make_unique<EffectValue>(-value, key, StatType::VALUE));
	currentValue = std::min(currentValue, baseValue + std::accumulate(extraValue.begin(), extraValue.end(), 0, [](int sum, const std::unique_ptr<EffectValue>& effect)
		{
			return sum + effect->value;
		}));
}