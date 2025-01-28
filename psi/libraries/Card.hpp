#pragma once

#include "Settings.hpp"
#include "Effect.hpp"
#include "Hero.hpp"

class Card : public Target
{
protected:
	uint_least32_t cardSeed;							// Seed of the card
	CardType cardType;
	void setCardType(const CardType cardType) { this->cardType = cardType; }

	//Visual elements of the card
	bool isFlipped = false;		// Flag to track if the card is flipped
	sf::Texture backTexture;	// Texture of the back of the card
	sf::Sprite back;			// string to location of the back of the card
	sf::Texture backgroundTexture;// Texture of the background of the card
	sf::Sprite background;		// string to location of the background of the card
	sf::Texture portraitTexture;// Texture of the portrait of the card
	sf::Sprite portrait;		// string to location of the portrait of the card
	sf::Texture frameTexture;	// Texture of the frame of the card
	sf::Sprite frame;			// string to location of the frame of the card

	sf::Font cardFont;
	unsigned int characterSize;
	const int cardWidth = 96;	// size of the card
	const int cardHeight = 128;	// size of the card
	sf::Vector2f position;

	// Common attributes of the card
	int baseEnergyCost;								// Mana cost of the card
	int currentEnergyCost;							// Current mana cost of the card

	bool isOnBoard = false;							// Flag	to track if the card is on the board

	Hero* owner = nullptr;							// Owner of the card
	Hero* enemy = nullptr;							// Enemy of the card

	std::vector<std::unique_ptr<Effect>> effects;	// List of effects

	std::vector<std::unique_ptr<IEffectBehavior>> activeEffects;	// List of active effects

	Card(const CardType& cardType) : cardType(cardType) {}
	std::mt19937 cardGenerator;
public:
	std::mt19937 getCardGenerator() const { return cardGenerator; }
	sf::FloatRect getBounds() const {
		return { static_cast<float>(position.x), static_cast<float>(position.y), static_cast<float>(cardWidth), static_cast<float>(cardHeight)
		};
	}
	std::vector<std::unique_ptr<EffectValue>> extraEnergyCost = {}; // Extra mana cost from effects

	static Card* createCard(uint_least32_t& cardSeed);
	~Card() override = default;

	bool getIsOnBoard() const { return isOnBoard; }
	void setIsOnBoard(bool value) { isOnBoard = value; }

	void setOwnerAndEnemy(Hero* owner, Hero* enemy) { this->owner = owner; this->enemy = enemy; }
	Hero* getOwner() const { return owner; }
	Hero* getEnemy() const { return enemy; }

	void reduceEnergyCost(int value, uint_least32_t key);
	void increaseEnergyCost(int value, uint_least32_t key);

	void applyEffect(std::unique_ptr<IEffectBehavior> effectBehavior);
	void removeEffect(IEffectBehavior* effectBehavior);

	void triggerEffect(EffectTrigger trigger, std::optional<GameEvent> event = std::nullopt);
	void triggerGameEvent(GameEvent event);

	uint_least32_t serialize() const { return cardSeed; }
	static Card* deserialize(uint_least32_t& data);

	uint_least32_t getSeed() const { return cardSeed; }
	void setFontAndFontSize(const std::string& fontPath, const unsigned int characterSize)
	{
		if (!cardFont.loadFromFile(fontPath)) {
			std::cerr << "Error: Could not load card font\n";
		}
		this->characterSize = characterSize;
	}
	sf::Vector2f getPosition() const { return position; }
	void setPosition(const sf::Vector2f pos) { position = pos; }
	void flip() { isFlipped = !isFlipped; }
	virtual void display(sf::RenderTexture& renderTexture) = 0;
	virtual void displayDetails(sf::RenderTexture& renderTexture) = 0;
	CardType getCardType() const { return cardType; }
	virtual void loadSprites(std::mt19937& cardGenerator) = 0;
	std::vector<std::string> getEffects() const
	{
		std::vector<std::string> effectDescriptions;
		effectDescriptions.reserve(effects.size());
		for (const auto& effect : effects)
		{
			effectDescriptions.push_back(effect->getDescription());
		}
		return effectDescriptions;
	}
};

class ItemCard final : public Card // If 0 durability, card is destroyed
{
protected:
	int baseDamage;
	int currentDamage;

	int baseDefense;
	int currentDefense;

	int baseDurability;
	int currentDurability;
	void loadSprites(std::mt19937& cardGenerator) override;
public:
	std::vector<std::unique_ptr<EffectValue>> extraDamage = {}; // Extra damage from effects
	std::vector<std::unique_ptr<EffectValue>> extraDefense = {}; // Extra defense from effects
	std::vector<std::unique_ptr<EffectValue>> extraDurability = {}; // Extra durability from effects

	void increaseDamage(int value, uint_least32_t key);
	void reduceDamage(int value, uint_least32_t key);
	int getDamage() const { return currentDamage; }
	int getBaseDamage() const { return baseDamage; }
	void increaseDefense(int value, uint_least32_t key);
	void reduceDefense(int value, uint_least32_t key);
	int getDefense() const { return currentDefense; }
	int getBaseDefense() const { return baseDefense; }
	void increaseDurability(int value, uint_least32_t key);
	void reduceDurability(int value, uint_least32_t key);
	int getDurability() const { return currentDurability; }
	int getBaseDurability() const { return baseDurability; }
	void iterateDurability();

	explicit ItemCard(uint_least32_t& cardSeed, std::mt19937& cardGenerator);
	~ItemCard() override = default;
	void destroy();
	void display(sf::RenderTexture& renderTexture) override;
	void displayDetails(sf::RenderTexture& renderTexture) override;
	void loadSprite(std::mt19937& cardGenerator) { loadSprites(cardGenerator); }
};

const std::string statusFolderPaths= "src/img/status icons/status_";
const std::string keywordFolderPaths = "src/img/keyword icons/keyword_";

class UnitCard final : public Card // If 0 health, card is destroyed
{
protected:
	std::vector<std::string> statusPaths = {
		statusFolderPaths + "bleeding.png",
		statusFolderPaths + "burning.png",
		statusFolderPaths + "confused.png",
		statusFolderPaths + "cursed.png",
		statusFolderPaths + "damaged.png",
		statusFolderPaths + "enraged.png",
		statusFolderPaths + "frozen.png",
		statusFolderPaths + "healing.png",
		statusFolderPaths + "hexed.png",
		statusFolderPaths + "marked.png",
		statusFolderPaths + "poisoned.png",
		statusFolderPaths + "rock_skinned.png",
		statusFolderPaths + "silenced.png",
		statusFolderPaths + "stunned.png"
	};
	std::vector<sf::Texture*> statusTextures;
	std::vector<std::string> keywordPaths = {
		keywordFolderPaths + "berserk.png",
		keywordFolderPaths + "bloodthirsty.png",
		keywordFolderPaths + "defender.png",
		keywordFolderPaths + "double_strike.png",
		keywordFolderPaths + "icebreaker.png",
		keywordFolderPaths + "immune.png",
		keywordFolderPaths + "instant_attack.png",
		keywordFolderPaths + "instant_killer.png",
		keywordFolderPaths + "life_steal.png",
		keywordFolderPaths + "master_of_arms.png",
		keywordFolderPaths + "pyromaniac.png",
		keywordFolderPaths + "ranged.png",
		keywordFolderPaths + "unstoppable.png"
	};
	std::vector<sf::Texture*> keywordTextures;
	int baseHealth;
	int currentHealth;

	int baseAttack;
	int currentAttack;

	std::unordered_set<Keyword> keywords;				// List of keywords
	std::map<Status, int> statuses;						// List of statuses
	std::unique_ptr<ItemCard> item = nullptr;			// Item card attached to the unit
	void loadSprites(std::mt19937& cardGenerator) override;
public:
	std::vector<std::unique_ptr<EffectValue>> extraHealth = {}; // Extra health from effects
	std::vector<std::unique_ptr<EffectValue>> extraAttack = {}; // Extra attack from effects

	void increaseHealth(int value, uint_least32_t key);
	void reduceHealth(int value, uint_least32_t key);
	int getHealth() const { return currentHealth; }
	void increaseAttack(int value, uint_least32_t key);
	void reduceAttack(int value, uint_least32_t key);
	int getAttack() const {
		if (item) {
			return currentAttack + item->getDamage();
		}
		return currentAttack;
	}

	void heal(int value);
	void dealDamage(int value);

	void applyStatus(const Status& status, int numberOfTurns);
	void removeStatus(const Status& status);

	void addKeyword(const Keyword& keyword);
	void removeKeyword(const Keyword& keyword);

	bool hasItem() const { return item != nullptr; }
	ItemCard* getItem() const { return item.get(); }

	explicit UnitCard(uint_least32_t& cardSeed, std::mt19937& cardGenerator);
	~UnitCard() override = default;
	void destroy();
	void display(sf::RenderTexture& renderTexture) override;
	void displayDetails(sf::RenderTexture& renderTexture) override;
	void loadSprite(std::mt19937& cardGenerator) { loadSprites(cardGenerator); }
};

class SpellCard final : public Card
{
protected:
	void loadSprites(std::mt19937& cardGenerator) override;
public:
	explicit SpellCard(uint_least32_t& cardSeed, std::mt19937& cardGenerator);
	~SpellCard() override = default;
	void display(sf::RenderTexture& renderTexture) override;
	void displayDetails(sf::RenderTexture& renderTexture) override;
	void loadSprite(std::mt19937& cardGenerator) { loadSprites(cardGenerator); }
};