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

class Player : public Hero, public boardGameMovable, public sf::Drawable, public sf::Transformable
{
private:
	int experience;

	sf::Sprite m_playerSprite;
	sf::Texture m_playerTexture;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		states.texture = &m_playerTexture;
		target.draw(m_playerSprite, states);
	}
public:
	Player() = default;
	int getExperience() const;
	void addExperience(const int value);

	bool load(const std::string& tileset, sf::Vector2i pathPos)
	{
		// load the tileset texture
		if (!m_playerTexture.loadFromFile(tileset))
			return false;

		m_playerSprite.setTexture(m_playerTexture);
		m_playerSprite.setTextureRect(sf::IntRect(32, 32, 16, 16));
		//AAAAAAAAAAAAAAAAAAAAAAAAAAAA
		m_playerSprite.setPosition((pathPos.x + 16) * pathPos.y, pathPos.y + 16);

		return true;
	}
};

class Enemy : public Hero
{
private:

public:
	Enemy() = default;
};