#pragma once

#include "Settings.hpp"
#include "Definitions.hpp"

class IEffectBehavior;

enum class EntityMovement
{
	STATIC,
	LOOP,
	PATROL
};

// Specifically for Board Game
class BoardGameMovable
{
private:
	sf::Vector2i mapPosition;
	sf::Sprite m_entitySprite;
	sf::Texture m_entityTexture;
public:
	sf::Vector2i getMapPosition() const { return this->mapPosition; }
	void setMapPosition(const sf::Vector2i& position) { this->mapPosition = position; }

	sf::Texture getEntityTexture() const { return this->m_entityTexture; }
	void setEntityTexture(const sf::Texture& texture) { m_entityTexture = texture; }

	// Load the entity sprite
	bool load(const std::string& tileset);
	sf::Sprite getSprite() const { return this->m_entitySprite; }
	void setSprite(const sf::Sprite& sprite) { m_entitySprite = sprite; }
};
// Wandering trader -> activation tiles: + 2 each side of path
// Enemy -> activation tiles: + 3 each side of path

// Specifically for Board Game
class BoardGamePlayer : public BoardGameMovable
{
private:
	int level;
	int abilityPoints;
	int experience;
	int money;

	void onLevelUp();
	int getRequiredXPForNextLevel() const;
public:
	int getTotalXPRequiredForNextLevel() const;

	BoardGamePlayer();
	BoardGamePlayer(const BoardGamePlayer& player);
	BoardGamePlayer& operator= (const BoardGamePlayer& player);

	void addExperience(int value);
	bool hasAvailableAbilityPoints() const;
	void buyAbility(const int abilityCost);

	// Getters and setters
	int getLevel() const { return level; }
	int getAbilityPoints() const { return abilityPoints; }
	int getExperience() const { return experience; }
	int getMoney() const { return money; }

	void setLevel(const int value) { level = value; }
	void setAbilityPoints(const int value) { abilityPoints = value; }
	void setExperience(const int value) { experience = value; }
	void setMoney(const int value) { money = value; }


	// Serialization and deserialization
	std::string serialize() const;
	static BoardGamePlayer* deserialize(const std::string& data);
};

class BoardGameEnemy : public BoardGameMovable
{
	EntityMovement movementType;
public:
	BoardGameEnemy();
	~BoardGameEnemy() = default;
	BoardGameEnemy(sf::Vector2i position);
	BoardGameEnemy(const BoardGameEnemy& enemy);
	BoardGameEnemy& operator= (const BoardGameEnemy& enemy);
	bool operator==(const BoardGameEnemy& enemy) const { return this->getMapPosition() == enemy.getMapPosition(); }

	EntityMovement getMovementType() const { return movementType; }
	void setMovementType(const EntityMovement type) { movementType = type; }

	std::string serialize() const;
	static BoardGameEnemy* deserialize(const std::string& data);
};

// Specifically for Card Game
class Hero : public Target
{
private:
	const int initialHandSizeToDraw = 4;
protected:
	int maxHealth = 25;				// Max value of health
	int currentHealth;				// Current health that is updated constantly

	int maxEnergy;					// Depending on turns
	int currentEnergy;				// Current energy stored in turn

	std::vector<Card*> deck;		// Deck that player has
	std::vector<Card*> hand;		// Hand that player has during turns
	std::vector<Card*> battlefield;	// Battlefield corresponding to player
	int fatigue = 1;				// Fatigue that player will draw when there are no cards in deck

	std::vector<std::unique_ptr<IEffectBehavior>> activeEffects;	// List of active effects(heal / damage )
public:
	Hero();

	std::vector<Card*> getDeck() const { return deck; }
	std::vector<Card*> getHand() const { return hand; }
	std::vector<Card*> getBattlefield() const { return battlefield; }


	void dealDamage(int value);
	void restoreHealth(int value);
	// used for drawing card t hand, in the game
	void drawCard();
	// used for discarding card from hand, in the game
	void discardCard();
	// used for shuffling card into the deck, in the game
	void shuffleCardIntoTheDeck(Card* card);

	// used to copy deck from save, before the game
	void copyDeck(std::vector<uint_least32_t> deck);
	// used to draw card from deck, before the game
	void drawInitialHand();

	void modifyEnergy(int value);

	void applyEffect(std::unique_ptr<IEffectBehavior> effectBehavior);
	void removeEffect(IEffectBehavior* effectBehavior);
};