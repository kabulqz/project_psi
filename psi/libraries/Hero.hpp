#pragma once

#include "Settings.hpp"
#include "Definitions.hpp"

class Hero : public Target
{
private:
	int maxHealth = 25;				// Max value of health
	int currentHealth;				// Current health that is updated constantly

	int maxEnergy;					// Depending on turns
	int currentEnergy;				// Current energy stored in turn

	std::stack<Card*> deck;		// Deck that player has
	std::vector<Card*> hand;		// Hand that player has during turns
	std::vector<Card*> battlefield;// Battlefield corresponding to player
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

class boardGameMovable
{
private:
	sf::Vector2i mapPosition;
public:
	sf::Vector2i getMapPosition() const;
	void setMapPosition(const sf::Vector2i& position);
};

class Player : public Hero, public boardGameMovable
{
private:
	int experience;
	int money;

	sf::Sprite m_playerSprite;
	sf::Texture m_playerTexture;
public:
	Player();
	int getExperience() const;
	void addExperience(const int value);
	int getMoney() const { return money; }
	void addMoney(const int value) { money += value; }
	sf::Sprite getSprite() { return this->m_playerSprite; }
	void setSprite(const sf::Sprite sprite) { m_playerSprite = sprite; }
	bool load(const std::string& tileset);

	std::string serialize() const;
	static Player* deserialize(const std::string& data);
};

class Enemy : public Hero
{
private:

public:
	Enemy() = default;
};
