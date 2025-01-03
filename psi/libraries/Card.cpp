#include "Card.hpp"

std::unique_ptr<Card> Card::createCard(const uint_least32_t& cardSeed)
{
	// Create a card based on the seed
	std::mt19937 randomEngine(cardSeed);
	std::uniform_int_distribution<int> cardTypeDistribution(0, 2);

	// Randomly select a card type
	switch (static_cast<CardType>(cardTypeDistribution(randomEngine))) {
	case CardType::UNIT:
		return std::make_unique<UnitCard>(randomEngine);
	case CardType::ITEM:
		return std::make_unique<ItemCard>(randomEngine);
	case CardType::SPELL:
		return std::make_unique<SpellCard>(randomEngine);
	}

	return nullptr;
}

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

UnitCard::UnitCard(std::mt19937& cardGenerator) :
	Card(CardType::UNIT) // Wywo³anie konstruktora bazowego
{
	// Wygeneruj wartoœci bazowe
	// Koszt energii (0 - 12)
	// (0 - 4) s³abe jednostki, (5 - 8) œrednie jednostki, (9 - 12) silne jednostki

	std::uniform_int_distribution<int> energyCostDistribution(0, 12);
	baseEnergyCost = energyCostDistribution(cardGenerator);
	currentEnergyCost = baseEnergyCost;

	// Bazowe zdrowie w zale¿noœci od kosztu energii (1 - 12)
	// Bazowy atak w zale¿noœci od kosztu energii (1 - 12)
	int minValDist, maxValDist;
	if (baseEnergyCost <= 4) {
		minValDist = 1;
		maxValDist = 4;
	}
	else if ( 4 < baseEnergyCost && baseEnergyCost <= 8) {
		minValDist = 5;
		maxValDist = 8;
	}
	else {
		minValDist = 9;
		maxValDist = 12;
	}
	std::uniform_int_distribution<int> healthDistribution(minValDist, maxValDist);
	std::uniform_int_distribution<int> attackDistribution(minValDist, maxValDist);
	baseHealth = healthDistribution(cardGenerator);
	baseAttack = attackDistribution(cardGenerator);
	currentHealth = baseHealth;
	currentAttack = baseAttack;

	// Liczba efektów, statusów i s³ów kluczowych
	// Efekty
	std::uniform_int_distribution<int> effectsDistribution(0, 3);
	const int numEffects = effectsDistribution(cardGenerator);
	for (int i = 0; i < numEffects; i++)
	{
		// Wygeneruj seed dla efektu z generatora kart
		std::uniform_int_distribution< uint_least32_t> effectSeedDistribution(std::mt19937::min(), std::mt19937::max());
		uint_least32_t effectSeed = effectSeedDistribution(cardGenerator);
		// Dodaj efekt na podstawie seeda i typu karty
		effects.push_back(std::make_unique<Effect>(effectSeed, cardType));
	}
	// Statusy
	std::uniform_int_distribution<int> statusesDistribution(0, 2);
	std::vector<Status> availableStatuses = {
		Status::CONFUSED,     // When attacking, target is random, even friendly
		Status::CURSED,		  // Cannot be healed or gain buffs while active
		Status::ENRAGED,	  // When damaged, gains attack
		Status::FROZEN,		  // Cannot attack
		Status::HEXED,		  // Cannot use effects from this unit
		Status::MARKED,		  // Takes extra damage
		Status::ROCK_SKINNED, // Takes reduced damage for example maximum of 1 or 50%
		Status::STUNNED		  // Cannot add items to this unit, if unit has already items, they are removed
	};
	std::ranges::shuffle(availableStatuses, cardGenerator);
	const int numStatuses = statusesDistribution(cardGenerator);
	for (int i = 0; i < numStatuses; i++)
	{
		statuses[availableStatuses[i]] = -1; // -1 oznacza, ¿e status nie ma limitu tur
	}
	// S³owa kluczowe
	std::uniform_int_distribution<int> keywordsDistribution(0, 2);
	std::vector<Keyword> availableKeywords = {
		Keyword::BERSERK,		 // Gains attack after it is attacked
		Keyword::BLOODTHIRSTY,	 // Gains attack whenever enemy unit has bleeding status
		Keyword::DEFENDER,		 // Must be attacked first
		Keyword::DOUBLE_STRIKE,	 // Can attack twice each turn
		Keyword::ICEBREAKER,	 // Gains health whenever it kills a frozen unit
		Keyword::IMMUNE,		 // Cannot be chosen as a target for effects
		Keyword::INSTANT_ATTACK, // Can attack immediately
		Keyword::INSTANT_KILLER, // Kills any unit it damages
		Keyword::LIFE_STEAL,	 // Heals the player when dealing damage
		Keyword::MASTER_OF_ARMS, // Gains attack whenever it is equipped with an item
		Keyword::PYROMANIAC,	 // Gains attack whenever enemy unit has burning status
		Keyword::RANGED,		 // When attacking, doesn't take damage back
		Keyword::UNSTOPPABLE	 // Ignores defenders, being frozen and stunned
	};
	std::ranges::shuffle(availableKeywords, cardGenerator);
	const int numKeywords = keywordsDistribution(cardGenerator);
	for (int i = 0; i < numKeywords; i++)
	{
		keywords.insert(availableKeywords[i]); // Dodaj s³owo kluczowe
	}
}

void UnitCard::destroy()
{// Destroy the unit card
	UnitCard::~UnitCard();
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

ItemCard::ItemCard(std::mt19937& cardGenerator) :
Card(CardType::ITEM)
{
	
}

void ItemCard::destroy()
{// Destroy the item card
	ItemCard::~ItemCard();
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

SpellCard::SpellCard(std::mt19937& cardGenerator) :
Card(CardType::SPELL)
{

}
