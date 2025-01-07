#include "Card.hpp"

Card* Card::createCard(const uint_least32_t& cardSeed)
{
	// Create a card based on the seed
	std::mt19937 randomEngine(cardSeed);
	std::uniform_int_distribution<int> cardTypeDistribution(0, 2);

	// Randomly select a card type
	switch (static_cast<CardType>(cardTypeDistribution(randomEngine))) {
	case CardType::UNIT:
		return new UnitCard(randomEngine);
	case CardType::ITEM:
		return new ItemCard(randomEngine);
	case CardType::SPELL:
		return new SpellCard(randomEngine);
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
		buffBehavior->removeEffect();
	}
	else if (auto* debuffBehavior = dynamic_cast<DebuffBehavior*>(effectBehavior))
	{
		debuffBehavior->removeEffect();
	}
	else if (auto* statusApplyBehavior = dynamic_cast<StatusApplyBehavior*>(effectBehavior))
	{
		statusApplyBehavior->removeEffect();
	}
	else if (auto* silenceBehavior = dynamic_cast<SilenceBehavior*>(effectBehavior))
	{
		silenceBehavior->removeEffect();
	}
	else  if (auto* keywordAddBehavior = dynamic_cast<KeywordAddBehavior*>(effectBehavior))
	{
		keywordAddBehavior->removeEffect();
	}

	// Remove the effect from the active effects list
	std::erase_if(activeEffects, [effectBehavior](const std::unique_ptr<IEffectBehavior>& effect)
		{
			return effect.get() == effectBehavior;
		});
}

void Card::triggerEffect(EffectTrigger trigger, std::optional<GameEvent> event)
{
	for (const auto& effect : effects)
	{
		// Execute the effect
		effect->executeEffect(this, trigger, event);
		// Inside this method we will check for potential activations of ON_GAME_EVENT effects from ally cards on battlefield
	}
}

void Card::triggerGameEvent(GameEvent event)
{
	// Trigger the effects that are set to trigger on game event
	triggerEffect(EffectTrigger::ON_GAME_EVENT, event);
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
	// Wygeneruj wartości bazowe
	// Koszt energii (0 - 12)
	// (0 - 4) słabe przedmioty, (5 - 8) średnie przedmioty, (9 - 12) silne przedmioty
	std::uniform_int_distribution<int> energyCostDistribution(0, 12);
	baseEnergyCost = energyCostDistribution(cardGenerator);
	currentEnergyCost = baseEnergyCost;

	// Bazowe obrażenia (1 - 6)
	// Bazowa obrona (1 - 6)
	// Bazowa wytrzymałość (1 - 3)
	int minValDist, maxValDist;
	if (baseEnergyCost <= 4) {
		minValDist = 1;
		maxValDist = 2;
	}
	else if (4 < baseEnergyCost && baseEnergyCost <= 8) {
		minValDist = 3;
		maxValDist = 4;
	}
	else {
		minValDist = 5;
		maxValDist = 6;
	}
	std::uniform_int_distribution<int> damageDistribution(minValDist, maxValDist);
	baseDamage = damageDistribution(cardGenerator);
	currentDamage = baseDamage;
	std::uniform_int_distribution<int> defenseDistribution(minValDist, maxValDist);
	baseDefense = defenseDistribution(cardGenerator);
	currentDefense = baseDefense;
	std::uniform_int_distribution<int> durabilityDistribution(1, 3);
	baseDurability = durabilityDistribution(cardGenerator);
	currentDurability = baseDurability;

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
}

void ItemCard::destroy()
{// Destroy the item card
	ItemCard::~ItemCard();
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

	// Zwiększ zdrowie o wartość, ale nie więcej niż maxHealth
	currentHealth = std::min(currentHealth + value, maxHealth);
}

void UnitCard::dealDamage(const int value)
{
	// Zmniejsz zdrowie o wartość, ale nie mniej niż 0
	currentHealth = std::max(0, currentHealth - value);
	if (currentHealth <= 0)
	{
		destroy();
	}
}

void UnitCard::applyStatus(const Status& status, const int numberOfTurns)
{
	// Jeśli jeszcze nie ma takiego statusu, dodaj go
	// Jeśli już jest taki status, zaktualizuj liczbę tur o numberOfTurns
	if (!statuses.contains(status))
	{
		statuses[status] = numberOfTurns;
	}
	else
	{
		statuses[status] += numberOfTurns;
	}
}

void UnitCard::removeStatus(const Status& status)
{
	// Jeśli status istnieje, usuń go
	if (statuses.contains(status))
	{
		statuses.erase(status);
	}
}

void UnitCard::addKeyword(const Keyword& keyword)
{
	// Je�li nie istnieje, dodaj nowe s�owo kluczowe do jednostki
	// Je�li istnieje, zignoruj
	if (!keywords.contains(keyword))
	{
		keywords.insert(keyword);
	}
}

void UnitCard::removeKeyword(const Keyword& keyword)
{
	// Je�li s�owo kluczowe istnieje, usu� je
	if (keywords.contains(keyword))
	{
		keywords.erase(keyword);
	}
}

UnitCard::UnitCard(std::mt19937& cardGenerator) :
	Card(CardType::UNIT) // Wywołanie konstruktora bazowego
{
	// Wygeneruj wartości bazowe
	// Koszt energii (0 - 12)
	// (0 - 4) słabe jednostki, (5 - 8) średnie jednostki, (9 - 12) silne jednostki

	std::uniform_int_distribution<int> energyCostDistribution(0, 12);
	baseEnergyCost = energyCostDistribution(cardGenerator);
	currentEnergyCost = baseEnergyCost;

	// Bazowe zdrowie w zależności od kosztu energii (1 - 12)
	// Bazowy atak w zależności od kosztu energii (1 - 12)
	int minValDist, maxValDist;
	if (baseEnergyCost <= 4) {
		minValDist = 1;
		maxValDist = 4;
	}
	else if (4 < baseEnergyCost && baseEnergyCost <= 8) {
		minValDist = 5;
		maxValDist = 8;
	}
	else {
		minValDist = 9;
		maxValDist = 12;
	}
	std::uniform_int_distribution<int> healthDistribution(minValDist, maxValDist);
	baseHealth = healthDistribution(cardGenerator);
	currentHealth = baseHealth;
	std::uniform_int_distribution<int> attackDistribution(minValDist, maxValDist);
	baseAttack = attackDistribution(cardGenerator);
	currentAttack = baseAttack;

	// Liczba efektów, statusów i słów kluczowych
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
		Status::MARKED,		  // Takes extra damage for example 50% more
		Status::ROCK_SKINNED, // Takes reduced damage for example maximum of 1 or 50%
		Status::STUNNED		  // Cannot add items to this unit, if unit has already items, they are removed
	};
	std::ranges::shuffle(availableStatuses, cardGenerator);
	const int numStatuses = statusesDistribution(cardGenerator);
	for (int i = 0; i < numStatuses; i++)
	{
		statuses[availableStatuses[i]] = -1; // -1 oznacza, że status nie ma limitu tur
	}
	// Słowa kluczowe
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
		keywords.insert(availableKeywords[i]); // Dodaj słowo kluczowe
	}
}

void UnitCard::destroy()
{// Destroy the unit card
	UnitCard::~UnitCard();
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
	// Wygeneruj wartości bazowe
	// Koszt energii (0 - 12)
	// (0 - 4) słabe zaklęcia, (5 - 8) średnie zaklęcia, (9 - 12) silne zaklęcia
	std::uniform_int_distribution<int> energyCostDistribution(0, 12);
	baseEnergyCost = energyCostDistribution(cardGenerator);
	currentEnergyCost = baseEnergyCost;

	// Value bedzie tylko potrzebne jesli Spell card bedzie mial efekt np. heal, dealDamage etc
	// inaczej nie bedzie potrzebne
	// Bazowa wartość (1 - 6)
	std::uniform_int_distribution<int> valueDistribution(1, 6);
	baseValue = valueDistribution(cardGenerator);
	currentValue = baseValue;

	// Efekty, Spell cardy mogą mieć od 2 do 6 efektów
	std::uniform_int_distribution<int> effectsDistribution(2, 6);
	const int numEffects = effectsDistribution(cardGenerator);
	for (int i = 0; i < numEffects; i++)
	{
		// Wygeneruj seed dla efektu z generatora kart
		std::uniform_int_distribution< uint_least32_t> effectSeedDistribution(std::mt19937::min(), std::mt19937::max());
		uint_least32_t effectSeed = effectSeedDistribution(cardGenerator);
		// Dodaj efekt na podstawie seeda i typu karty
		effects.push_back(std::make_unique<Effect>(effectSeed, cardType));
	}
}
