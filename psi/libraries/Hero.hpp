#pragma once

#include "Settings.hpp"
#include "Definitions.hpp"

// Specifically for Board Game
class BoardGameMovable
{
private:
	sf::Vector2i mapPosition;
public:
	sf::Vector2i getMapPosition() const { return this->mapPosition; }
	void setMapPosition(const sf::Vector2i& position) { this->mapPosition = position; }
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

	sf::Sprite m_playerSprite;
	sf::Texture m_playerTexture;

	void onLevelUp();
	int getRequiredXPForNextLevel() const;
public:
	int getTotalXPRequiredForNextLevel() const;

	BoardGamePlayer();

	void addExperience(int value);

	// Getters and setters
	int getLevel() const { return level; }
	int getAbilityPoints() const { return abilityPoints; }
	int getExperience() const { return experience; }
	int getMoney() const { return money; }

	void setLevel(const int value) { level = value; }
	void setAbilityPoints(const int value) { abilityPoints = value; }
	void setExperience(const int value) { experience = value; }
	void setMoney(const int value) { money = value; }

	// Load the player sprite
	bool load(const std::string& tileset);
	sf::Sprite getSprite() { return this->m_playerSprite; }
	void setSprite(const sf::Sprite& sprite) { m_playerSprite = sprite; }

	// Serialization and deserialization
	std::string serialize() const;
	static BoardGamePlayer* deserialize(const std::string& data);
};

// Specifically for Card Game
class Hero : public Target
{
private:
	int maxHealth = 25;				// Max value of health
	int currentHealth;				// Current health that is updated constantly

	int maxEnergy;					// Depending on turns
	int currentEnergy;				// Current energy stored in turn

	std::stack<Card*> deck;			// Deck that player has
	std::vector<Card*> hand;		// Hand that player has during turns
	std::vector<Card*> battlefield;	// Battlefield corresponding to player
	int fatigue = 1;				// Fatigue that player will draw when there are no cards in deck
public:
	Hero();
	Hero(const Hero& hero);
	Hero& operator=(const Hero& hero);

	std::stack<Card*> getDeck() const;
	std::vector<Card*> getHand() const;
	std::vector<Card*> getBattlefield() const;

	void dealDamage(int value);
	void restoreHealth(int value);
	void drawCard();
	void shuffleCardIntoTheDeck(Card* card);
};

// Specifically for Card Game
class Enemy : public Hero
{
private:

public:
	Enemy() = default;
};

// Specifically for Card Game
class Player : public Hero
{
private:
	int experience;
	int money;
public:
	Player();
};