#include "Card.hpp"

#include <ranges>

const std::string CardFolderPath = "src/img/card/";

static std::string cardTypeToString(const CardType cardType)
{
	switch (cardType)
	{
	case CardType::UNIT:
		return "UNIT";
	case CardType::ITEM:
		return "ITEM";
	case CardType::SPELL:
		return "SPELL";
	}
	return "";
}

static void drawValueCircle(sf::RenderTexture& renderTexture, const sf::Font& cardFont, int characterSize, float circleRadius, const sf::Color circleColor, const sf::Vector2f&position, int currentValue, int baseValue)
{
	sf::CircleShape valueCircle(circleRadius);
	valueCircle.setFillColor(circleColor);
	valueCircle.setOutlineThickness(2.0f);
	valueCircle.setOutlineColor(sf::Color::White);
	valueCircle.setPosition(position);
	renderTexture.draw(valueCircle);

	sf::Text valueText(std::to_string(currentValue), cardFont, characterSize);
	if (currentValue < baseValue)
	{
		valueText.setFillColor(sf::Color::Red);
	}
	else if (currentValue > baseValue)
	{
		valueText.setFillColor(sf::Color::Green);
	}
	else
	{
		valueText.setFillColor(sf::Color::White);
	}
	sf::FloatRect textBounds = valueText.getLocalBounds();
	sf::Vector2f circleCenter = sf::Vector2f(
		valueCircle.getPosition().x + valueCircle.getRadius(),
		valueCircle.getPosition().y + valueCircle.getRadius()
	);
	valueText.setOrigin(textBounds.left + textBounds.width / 2.0f,
		textBounds.top + textBounds.height / 2.0f);
	valueText.setPosition(circleCenter);
	renderTexture.draw(valueText);
}

Card* Card::createCard(uint_least32_t& cardSeed)
{
	// Create a card based on the seed
	std::mt19937 randomEngine(cardSeed);
	std::uniform_int_distribution<int> cardTypeDistribution(static_cast<int>(CardType::UNIT), static_cast<int>(CardType::SPELL));

	// Randomly select a card type
	switch (const auto choice = static_cast<CardType>(cardTypeDistribution(randomEngine))) {
	case CardType::UNIT:
		return new UnitCard(cardSeed, randomEngine);
	case CardType::ITEM:
		return new ItemCard(cardSeed, randomEngine);
	case CardType::SPELL:
		return new SpellCard(cardSeed, randomEngine);
	default:
		std::cerr << "Error: Invalid card type generated: "
			<< cardTypeToString(choice) + "\n";
		return nullptr; // Zwróć nullptr, jeśli wystąpi błąd
	}
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

Card* Card::deserialize(uint_least32_t& data)
{
	return createCard(data);
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

void ItemCard::iterateDurability()
{
	--currentDurability;
	if (currentDurability <= 0)
	{
		destroy();
	}
}

ItemCard::ItemCard(uint_least32_t& cardSeed, std::mt19937& cardGenerator) :
Card(CardType::ITEM)
{
	this->cardGenerator = cardGenerator;
	this->cardSeed = cardSeed;
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
		auto effect = std::make_unique<Effect>(effectSeed, cardType);
		if (!effect->getDescription().empty()) {
			effects.push_back(std::move(effect));
			//std::cout << "\nCardType: " + cardTypeToString(cardType) + ", EffectSeed: " + std::to_string(effectSeed) + +"\n" + effects.back()->getDescription() + "\n";
		}
	}

	loadSprites(cardGenerator);
}

void ItemCard::destroy()
{// Destroy the item card
	triggerGameEvent(GameEvent::ITEM_DESTROYED);
	ItemCard::~ItemCard();
}

void ItemCard::loadSprites(std::mt19937& cardGenerator)
{
	// BACK
	if (!backTexture.loadFromFile(CardFolderPath + "back/" + "item.png")) {
		std::cerr << "Error: Could not load item card image\n";
	}
	back.setTexture(backTexture);

	// BACKGROUND
	std::uniform_int_distribution<int> backgroundDistribution(1, 21);
	if (const int choice = backgroundDistribution(cardGenerator);  !backgroundTexture.loadFromFile(CardFolderPath + "background/" + std::to_string(choice) + ".png")) {
		std::cerr << "Error: Could not load item background image\n";
	}
	background.setTexture(backgroundTexture);


	// PORTRAIT
	if (!portraitTexture.loadFromFile(CardFolderPath + "portrait/item/" + "items.png")) {
		std::cerr << "Error: Could not load item image table\n";
	}
	constexpr int itemTextureSize = 16;
	std::uniform_int_distribution<int> columnDistribution(0, portraitTexture.getSize().x / itemTextureSize - 1);
	std::uniform_int_distribution<int> rowDistribution(0, portraitTexture.getSize().y / itemTextureSize - 1);
	const int column = columnDistribution(cardGenerator);
	const int row = rowDistribution(cardGenerator);
	const sf::IntRect itemTextureRect(column * itemTextureSize, row * itemTextureSize, itemTextureSize, itemTextureSize);
	portrait.setTexture(portraitTexture);
	portrait.setTextureRect(itemTextureRect);
	portrait.setScale(3.0f, 3.0f);  // from 16x16 to 48x48

	// FRAME
	if (!frameTexture.loadFromFile(CardFolderPath + "frame/" + "frame.png")) {
		std::cerr << "Error: Could not load item frame image\n";
	}
	frame.setTexture(frameTexture);
}

void ItemCard::display(sf::RenderTexture& renderTexture)
{
	if (!isFlipped)
	{
		back.setPosition(position);
		back.setScale(1.0f, 1.0f);
		renderTexture.draw(back);
	}
	else
	{
		// draw background
		background.setPosition(position);
		background.setScale(1.0f, 1.0f);
		renderTexture.draw(background);
		//draw portrait
		constexpr float portraitWidth = 48.0f;
		constexpr float portraitHeight = 48.0f;
		float portraitX = position.x + (cardWidth - portraitWidth) / 2.0f;
		float portraitY = position.y + (cardHeight - portraitHeight) / 2.0f;
		portrait.setPosition(portraitX, portraitY);
		portrait.setScale(3.0f, 3.0f);
		renderTexture.draw(portrait);
		//draw frame
		frame.setPosition(position);
		frame.setScale(1.0f, 1.0f);
		renderTexture.draw(frame);

		//energy cost
		drawValueCircle(renderTexture, 
			cardFont, 
			characterSize, 
			10.0f, 
			sf::Color(255, 159, 28), 
			sf::Vector2f((cardWidth / 2) - 10.0f + position.x, 8 + position.y), 
			currentEnergyCost, 
			baseEnergyCost);

		// damage, durability, defense
		constexpr float circleRadius = 10.0f;
		const float yPosition = position.y + 105.0f;
		const float spacing = (cardWidth - 3 * 2 * circleRadius) / 4.0f;

		drawValueCircle(renderTexture,
			cardFont,
			characterSize,
			circleRadius,
			sf::Color(244, 151, 218),
			sf::Vector2f(position.x + spacing, yPosition),
			currentDamage,
			baseDamage);

		drawValueCircle(renderTexture,
			cardFont,
			characterSize,
			circleRadius,
			sf::Color(200, 200, 200),
			sf::Vector2f(position.x + spacing * 2 + 2 * circleRadius, yPosition),
			currentDurability,
			baseDurability);

		drawValueCircle(renderTexture,
			cardFont,
			characterSize,
			circleRadius,
			sf::Color(133, 199, 242),
			sf::Vector2f(position.x + spacing * 3 + 4 * circleRadius, yPosition),
			currentDefense,
			baseDefense);
	}
}

void ItemCard::displayDetails(sf::RenderTexture& renderTexture)
{
	// Nowe przesunięcie
	sf::Vector2f offset(-24.f, -32.f);

	if (!isFlipped)
	{
		back.setPosition(position + offset);
		back.setScale(1.5f, 1.5f);
		renderTexture.draw(back);
	}
	else
	{
		// Rysowanie tła
		background.setPosition(position + offset);
		background.setScale(1.5f, 1.5f);
		renderTexture.draw(background);

		// Rysowanie portretu
		constexpr float portraitWidth = 48.0f;
		constexpr float portraitHeight = 48.0f;
		float portraitX = position.x + (cardWidth - portraitWidth) / 2.0f + offset.x;
		float portraitY = position.y + (cardHeight - portraitHeight) / 2.0f + offset.y;
		portrait.setPosition(portraitX, portraitY);
		portrait.setScale(3.0f * 1.5f, 3.0f * 1.5f);  // Skalowanie 1.5x
		renderTexture.draw(portrait);

		// Rysowanie ramki
		frame.setPosition(position + offset);
		frame.setScale(1.5f, 1.5f);
		renderTexture.draw(frame);

		// Rysowanie kosztu energii
		drawValueCircle(renderTexture,
			cardFont,
			characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
			10.0f * 1.5f,  // Zwiększenie średnicy okręgu o 1.5x
			sf::Color(255, 159, 28),
			sf::Vector2f((cardWidth * 1.5f / 2) - 15.0f + position.x + offset.x, 8.0f * 1.5f + position.y + offset.y),
			currentEnergyCost,
			baseEnergyCost);

		// Obliczanie odstępów między kołami
		constexpr float circleRadius = 10.0f * 1.5f;  // Skalowanie promienia koła
		const float yPosition = position.y + 105.0f * 1.5f;  // Przesunięcie Y z uwzględnieniem skalowania
		const float spacing = (cardWidth * 1.5f - 3 * 2 * circleRadius) / 4.0f;

		// Rysowanie kół z nowymi odstępami
		drawValueCircle(renderTexture,
			cardFont,
			characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
			circleRadius,
			sf::Color(244, 151, 218),
			sf::Vector2f(position.x + spacing + offset.x, yPosition + offset.y),
			currentDamage,
			baseDamage);

		drawValueCircle(renderTexture,
			cardFont,
			characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
			circleRadius,
			sf::Color(200, 200, 200),
			sf::Vector2f(position.x + spacing * 2 + 2 * circleRadius + offset.x, yPosition + offset.y),
			currentDurability,
			baseDurability);

		drawValueCircle(renderTexture,
			cardFont,
			characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
			circleRadius,
			sf::Color(133, 199, 242),
			sf::Vector2f(position.x + spacing * 3 + 4 * circleRadius + offset.x, yPosition + offset.y),
			currentDefense,
			baseDefense);
	}
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
	int newValue = value;
	if (item) {
		newValue = value - item->getDefense();
		item->iterateDurability();
	}
	if (statuses.contains(Status::MARKED)) {
		newValue = value + value / 2;
	}
	if (statuses.contains(Status::ROCK_SKINNED)){
		newValue = 1;
	}

	// Zmniejsz zdrowie o wartość, ale nie mniej niż 0
	currentHealth = std::max(0, currentHealth - newValue);
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
		if (status == Status::STUNNED)
		{
			// Jeśli jednostka jest ogłuszona, usuń wszystkie przedmioty
			if (item) {
				item->destroy();
			}
			item = nullptr;
		}
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

UnitCard::UnitCard(uint_least32_t& cardSeed, std::mt19937& cardGenerator) :
	Card(CardType::UNIT) // Wywołanie konstruktora bazowego
{
	this->cardGenerator = cardGenerator;
	this->cardSeed = cardSeed;

	for (const auto& statusPath : statusPaths)
	{
		auto texture = new sf::Texture();
		if (!texture->loadFromFile(statusPath))
		{
			std::cerr << "Error: Could not load status image\n";
		}
		statusTextures.push_back(texture);
	}

	for (const auto& keywordPath : keywordPaths)
	{
		auto texture = new sf::Texture();
		if (!texture->loadFromFile(keywordPath))
		{
			std::cerr << "Error: Could not load keyword image\n";
		}
		keywordTextures.push_back(texture);
	}

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
		auto effect = std::make_unique<Effect>(effectSeed, cardType);
		if (!effect->getDescription().empty()) {
			effects.push_back(std::move(effect));
			//std::cout << "\nCardType: " + cardTypeToString(cardType) +", EffectSeed: " + std::to_string(effectSeed) + +"\n" + effects.back()->getDescription() + "\n";
		}
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

	loadSprites(cardGenerator);
}

void UnitCard::destroy()
{// Destroy the unit card
	triggerGameEvent(GameEvent::UNIT_DEATH);
	UnitCard::~UnitCard();
}

void UnitCard::loadSprites(std::mt19937& cardGenerator)
{
	// BACK
	if (!backTexture.loadFromFile(CardFolderPath + "back/" + "unit.png")) {
		std::cerr << "Error: Could not load unit card image\n";
	}
	back.setTexture(backTexture);

	// BACKGROUND
	std::uniform_int_distribution<int> backgroundDistribution(1, 21);
	if (const int choice = backgroundDistribution(cardGenerator);  !backgroundTexture.loadFromFile(CardFolderPath + "background/" + std::to_string(choice) + ".png")) {
		std::cerr << "Error: Could not load item background image\n";
	}
	background.setTexture(backgroundTexture);

	// PORTRAIT
	std::uniform_int_distribution<int> distribution(1, 170);
	if (const int choice = distribution(cardGenerator); !portraitTexture.loadFromFile(CardFolderPath + "portrait/unit/" + std::to_string(choice) + ".png")) {
		std::cerr << "Error: Could not load unit image\n";
	}
	portrait.setTexture(portraitTexture);

	// FRAME
	if (keywords.contains(Keyword::DEFENDER)) {
		if (!frameTexture.loadFromFile(CardFolderPath + "frame/" + "defender.png")) {
			std::cerr << "Error: Could not load defender frame image\n";
		}
	}
	else {
		if (!frameTexture.loadFromFile(CardFolderPath + "frame/" + "frame.png")) {
			std::cerr << "Error: Could not load unit frame image\n";
		}
	}
	frame.setTexture(frameTexture);
}

void UnitCard::display(sf::RenderTexture& renderTexture)
{
	if (!isFlipped)
	{
		back.setPosition(position);
		back.setScale(1.0f, 1.0f);
		renderTexture.draw(back);
	}
	else
	{
		// draw background
		background.setPosition(position);
		background.setScale(1.0f, 1.0f);
		renderTexture.draw(background);
		//draw portrait
		portrait.setPosition(position);
		portrait.setScale(1.0f, 1.0f);
		renderTexture.draw(portrait);
		//draw frame
		frame.setPosition(position);
		frame.setScale(1.0f, 1.0f);
		renderTexture.draw(frame);

		//energy cost
		drawValueCircle(renderTexture,
			cardFont,
			characterSize,
			10.0f,
			sf::Color(255, 159, 28),
			sf::Vector2f((cardWidth / 2) - 10.0f + position.x, 8 + position.y),
			currentEnergyCost,
			baseEnergyCost);

		constexpr float circleRadius = 10.0f;
		const float yPosition = position.y + 105.0f;
		const float spacing = (cardWidth - 3 * 2 * circleRadius) / 4.0f;

		if (hasItem()) {// if unit has item
			// item -> damage, durability, defense
			drawValueCircle(renderTexture,
				cardFont,
				characterSize,
				circleRadius,
				sf::Color(244, 151, 218),
				sf::Vector2f(position.x + spacing, yPosition),
				item->getDamage(),
				item->getBaseDamage());

			drawValueCircle(renderTexture,
				cardFont,
				characterSize,
				circleRadius,
				sf::Color(200, 200, 200),
				sf::Vector2f(position.x + spacing * 2 + 2 * circleRadius, yPosition),
				item->getDurability(),
				item->getBaseDurability());

			drawValueCircle(renderTexture,
				cardFont,
				characterSize,
				circleRadius,
				sf::Color(133, 199, 242),
				sf::Vector2f(position.x + spacing * 3 + 4 * circleRadius, yPosition),
				item->getDefense(),
				item->getBaseDefense());

			// attack, health
			constexpr float unitStatsYOffset = -circleRadius * 2.0f - 1;
			drawValueCircle(renderTexture,
				cardFont,
				characterSize,
				10.0f,
				sf::Color(100, 149, 237),
				sf::Vector2f(position.x + spacing, yPosition + unitStatsYOffset),
				currentAttack,
				baseAttack);

			drawValueCircle(renderTexture,
				cardFont,
				characterSize,
				10.0f,
				sf::Color(255, 69, 0),
				sf::Vector2f(position.x + spacing * 3 + 4 * circleRadius, yPosition + unitStatsYOffset),
				currentHealth,
				baseHealth);
		}
		else {// unit does not have item
			// attack, health
			drawValueCircle(renderTexture,
				cardFont,
				characterSize,
				10.0f,
				sf::Color(100, 149, 237),
				sf::Vector2f(position.x + spacing, yPosition),
				currentAttack,
				baseAttack);

			drawValueCircle(renderTexture,
				cardFont,
				characterSize,
				10.0f,
				sf::Color(255, 69, 0),
				sf::Vector2f(position.x + spacing * 3 + 4 * circleRadius, yPosition),
				currentHealth,
				baseHealth);
		}
	}
}

void UnitCard::displayDetails(sf::RenderTexture& renderTexture)
{
	// Nowe przesunięcie
	sf::Vector2f offset(-24.f, -32.f);

	if (!isFlipped)
	{
		back.setPosition(position + offset);
		back.setScale(1.5f, 1.5f);
		renderTexture.draw(back);
	}
	else
	{
		// Rysowanie tła
		background.setPosition(position + offset);
		background.setScale(1.5f, 1.5f);
		renderTexture.draw(background);
		// Rysowanie portretu
		portrait.setPosition(position + offset);
		portrait.setScale(1.5f, 1.5f);
		renderTexture.draw(portrait);
		// Rysowanie ramki
		frame.setPosition(position + offset);
		frame.setScale(1.5f, 1.5f);
		renderTexture.draw(frame);

		// Rysowanie kosztu energii
		drawValueCircle(renderTexture,
			cardFont,
			characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
			10.0f * 1.5f,  // Zwiększenie średnicy okręgu o 1.5x
			sf::Color(255, 159, 28),
			sf::Vector2f((cardWidth * 1.5f / 2) - 15.f + position.x + offset.x, 8.f * 1.5f + position.y + offset.y),
			currentEnergyCost,
			baseEnergyCost);

		// Obliczanie odstępów między kołami
		constexpr float circleRadius = 10.0f * 1.5f;  // Skalowanie promienia koła
		const float yPosition = position.y + 105.0f * 1.5f;  // Przesunięcie Y z uwzględnieniem skalowania
		const float spacing = (cardWidth * 1.5f - 3 * 2 * circleRadius) / 4.0f;

		float statusIconX = position.x - 24.0f + 144.f;  // Startujemy zaraz po szerokości karty
		const float statusIconY = position.y - 32.f;  // Nowa wysokość dla statusów

		for (const auto& key : statuses | std::views::keys)
		{
			if (const int statusIndex = static_cast<int>(key); statusIndex < statusTextures.size()) {
				sf::Sprite statusSprite;
				statusSprite.setTexture(*statusTextures[statusIndex]);
				statusSprite.setPosition(statusIconX, statusIconY);
				statusSprite.setScale(0.5f, 0.5f);
				renderTexture.draw(statusSprite);
				statusIconX += statusTextures[statusIndex]->getSize().x * 0.5f + 2.f;
			}
		}

		float keywordIconX = position.x - 24.0f + 144.f;
		const float keywordIconY = position.y - 32.0f + 50.0f;

		for (const auto& key : keywords)
		{
			if (const int keywordIndex = static_cast<int>(key); keywordIndex < keywordTextures.size()) {
				sf::Sprite keywordSprite;
				keywordSprite.setTexture(*keywordTextures[keywordIndex]);
				keywordSprite.setPosition(keywordIconX, keywordIconY);
				keywordSprite.setScale(0.5f, 0.5f);
				renderTexture.draw(keywordSprite);
				keywordIconX += keywordTextures[keywordIndex]->getSize().x * 0.5f + 2.f;
			}
		}

		if (hasItem()) // Jeśli jednostka ma przedmiot
		{
			// Przedmiot -> obrażenia, trwałość, obrona
			drawValueCircle(renderTexture,
				cardFont,
				characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
				circleRadius * 1.5f,  // Zwiększenie promienia okręgu o 1.5x
				sf::Color(244, 151, 218),
				sf::Vector2f(position.x + spacing, yPosition),
				item->getDamage(),
				item->getBaseDamage());

			drawValueCircle(renderTexture,
				cardFont,
				characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
				circleRadius * 1.5f,  // Zwiększenie promienia okręgu o 1.5x
				sf::Color(200, 200, 200),
				sf::Vector2f(position.x + spacing * 2 + 2 * circleRadius, yPosition),
				item->getDurability(),
				item->getBaseDurability());

			drawValueCircle(renderTexture,
				cardFont,
				characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
				circleRadius * 1.5f,  // Zwiększenie promienia okręgu o 1.5x
				sf::Color(133, 199, 242),
				sf::Vector2f(position.x + spacing * 3 + 4 * circleRadius, yPosition),
				item->getDefense(),
				item->getBaseDefense());

			// Atak, zdrowie
			const float unitStatsYOffset = -circleRadius * 3.0f - 1; // Przesunięcie dla statystyk jednostki
			drawValueCircle(renderTexture,
				cardFont,
				characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
				10.0f * 1.5f,  // Zwiększenie średnicy okręgu o 1.5x
				sf::Color(100, 149, 237),
				sf::Vector2f(position.x + spacing, yPosition + unitStatsYOffset),
				currentAttack,
				baseAttack);

			drawValueCircle(renderTexture,
				cardFont,
				characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
				10.0f * 1.5f,  // Zwiększenie średnicy okręgu o 1.5x
				sf::Color(255, 69, 0),
				sf::Vector2f(position.x + spacing * 3 + 4 * circleRadius, yPosition + unitStatsYOffset),
				currentHealth,
				baseHealth);
		}
		else // Jednostka nie ma przedmiotu
		{
			// Atak, zdrowie
			drawValueCircle(renderTexture,
				cardFont,
				characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
				10.0f * 1.5f,  // Zwiększenie średnicy okręgu o 1.5x
				sf::Color(100, 149, 237),
				sf::Vector2f(position.x + spacing + offset.x, yPosition + offset.y),
				currentAttack,
				baseAttack);

			drawValueCircle(renderTexture,
				cardFont,
				characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
				10.0f * 1.5f,  // Zwiększenie średnicy okręgu o 1.5x
				sf::Color(255, 69, 0),
				sf::Vector2f(position.x + spacing * 3 + 4 * circleRadius + offset.x, yPosition + offset.y),
				currentHealth,
				baseHealth);
		}
	}
}

SpellCard::SpellCard(uint_least32_t& cardSeed, std::mt19937& cardGenerator) :
Card(CardType::SPELL)
{
	this->cardGenerator = cardGenerator;
	this->cardSeed = cardSeed;
	// Wygeneruj wartości bazowe
	// Koszt energii (0 - 12)
	// (0 - 4) słabe zaklęcia, (5 - 8) średnie zaklęcia, (9 - 12) silne zaklęcia
	std::uniform_int_distribution<int> energyCostDistribution(0, 12);
	baseEnergyCost = energyCostDistribution(cardGenerator);
	currentEnergyCost = baseEnergyCost;

	// Efekty, Spell cardy mogą mieć od 2 do 6 efektów
	std::uniform_int_distribution<int> effectsDistribution(2, 6);
	const int numEffects = effectsDistribution(cardGenerator);
	for (int i = 0; i < numEffects; i++)
	{
		// Wygeneruj seed dla efektu z generatora kart
		std::uniform_int_distribution< uint_least32_t> effectSeedDistribution(std::mt19937::min(), std::mt19937::max());
		uint_least32_t effectSeed = effectSeedDistribution(cardGenerator);
		// Dodaj efekt na podstawie seeda i typu karty
		auto effect = std::make_unique<Effect>(effectSeed, cardType);
		if (!effect->getDescription().empty()) {
			effects.push_back(std::move(effect));
			//std::cout << "\nCardType: " + cardTypeToString(cardType) + ", EffectSeed: " + std::to_string(effectSeed) + +"\n" + effects.back()->getDescription() + "\n";
		}
	}

	loadSprites(cardGenerator);
}

void SpellCard::loadSprites(std::mt19937& cardGenerator)
{
	// BACK
	if (!backTexture.loadFromFile(CardFolderPath + "back/" + "spell.png")) {
		std::cerr << "Error: Could not load spell card image\n";
	}
	back.setTexture(backTexture);

	// BACKGROUND
	// no need for background for spell cards

	// PORTRAIT
	std::uniform_int_distribution<int> distribution(1, 11);
	if (const int choice = distribution(cardGenerator); !portraitTexture.loadFromFile(CardFolderPath + "portrait/spell/" + std::to_string(choice) + ".png")) {
		std::cerr << "Error: Could not load spell image\n";
	}
	portrait.setTexture(portraitTexture);

	// FRAME
	if (!frameTexture.loadFromFile(CardFolderPath + "frame/" + "frame.png")) {
		std::cerr << "Error: Could not load spell frame image\n";
	}
	frame.setTexture(frameTexture);
}

void SpellCard::display(sf::RenderTexture& renderTexture)
{
	if (!isFlipped)
	{
		back.setPosition(position);
		back.setScale(1.0f, 1.0f);
		renderTexture.draw(back);
	}
	else
	{
		// draw background
		// no need for background for spell cards
		background.setScale(1.0f, 1.0f);
		renderTexture.draw(background);
		//draw portrait
		portrait.setPosition(position);
		portrait.setScale(1.0f, 1.0f);
		renderTexture.draw(portrait);
		//draw frame
		frame.setPosition(position);
		frame.setScale(1.0f, 1.0f);
		renderTexture.draw(frame);

		//energy cost
		drawValueCircle(renderTexture,
			cardFont,
			characterSize,
			10.0f,
			sf::Color(255, 159, 28),
			sf::Vector2f((cardWidth / 2) - 10.0f + position.x, 8 + position.y),
			currentEnergyCost,
			baseEnergyCost);
	}
}

void SpellCard::displayDetails(sf::RenderTexture& renderTexture)
{
	// Nowe przesunięcie
	sf::Vector2f offset(-24.f, -32.f);

	if (!isFlipped)
	{
		back.setPosition(position + offset);
		back.setScale(1.5f, 1.5f);
		renderTexture.draw(back);
	}
	else
	{
		// Rysowanie tła (jeśli jest potrzebne)
		background.setPosition(position + offset);
		background.setScale(1.5f, 1.5f);
		renderTexture.draw(background);

		// Rysowanie portretu
		portrait.setPosition(position + offset);
		portrait.setScale(1.5f, 1.5f);
		renderTexture.draw(portrait);

		// Rysowanie ramki
		frame.setPosition(position + offset);
		frame.setScale(1.5f, 1.5f);
		renderTexture.draw(frame);

		// Rysowanie kosztu energii
		drawValueCircle(
			renderTexture,
			cardFont,
			characterSize * 1.5f,  // Zwiększenie rozmiaru czcionki o 1.5x
			10.f * 1.5f,  // Zwiększenie średnicy okręgu o 1.5x
			sf::Color(255, 159, 28),
			sf::Vector2f((cardWidth * 1.5f / 2) - 15.f + position.x + offset.x, 8.f * 1.5f + position.y + offset.y),
			currentEnergyCost,
			baseEnergyCost
		);
	}
}
