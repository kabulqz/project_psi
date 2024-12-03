#include "Hero.hpp"

#include <ranges>
#include "Card.hpp"

Hero::Hero()
{
	// Write checks for the deck, hand and battlefield
	hand.reserve(10);
	battlefield.reserve(5);
}

Hero::Hero(const Hero& hero)
{
	maxHealth = hero.maxHealth;
	currentHealth = hero.currentHealth;
	maxEnergy = hero.maxEnergy;
	currentEnergy = hero.currentEnergy;
	fatigue = hero.fatigue;
	deck = hero.deck;
	hand = hero.hand;
	battlefield = hero.battlefield;
}

Hero& Hero::operator=(const Hero& hero)
{
	if (this == &hero) return *this;

	maxHealth = hero.maxHealth;
	currentHealth = hero.currentHealth;
	maxEnergy = hero.maxEnergy;
	currentEnergy = hero.currentEnergy;
	fatigue = hero.fatigue;
	deck = hero.deck;
	hand = hero.hand;
	battlefield = hero.battlefield;
	return *this;
}

std::stack<Card*> Hero::getDeck() const
{
	return deck;
}

std::vector<Card*> Hero::getHand() const
{
	return hand;
}

std::vector<Card*> Hero::getBattlefield() const
{
	return battlefield;
}

void Hero::dealDamage(int value)
{
	currentHealth -= value;
	currentHealth = std::max(0, currentHealth);
	if (currentHealth == 0)
	{
		// Game over
	}
}

void Hero::restoreHealth(int value)
{
	currentHealth += value;
	currentHealth = std::min(currentHealth, maxHealth);
}

void Hero::drawCard()
{
	if (deck.empty())
	{
		// Fatigue
		dealDamage(fatigue);
		fatigue++;
	}
	else
	{
		Card* card = deck.top();
		hand.push_back(card);
		deck.pop();
		card->setZone(TargetZone::HAND);
	}
}

void Hero::shuffleCardIntoTheDeck(Card* card)
{
	if (!card) return; // For safety

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(0, deck.size()); // Uniform distribution to get random position

	int randomIndex = distribution(generator); // Get a random index	within the deck size

	// Step 1: Convert the deck from a stack to a vector temporarily to allow random access
	std::vector<Card*> tempDeck;
	while (!deck.empty())
	{
		tempDeck.push_back(deck.top());
		deck.pop();
	}

	// Step 2: Insert the card at the random position
	tempDeck.insert(tempDeck.begin() + randomIndex, card);

	// Step 3: Convert the deck back to a stack
	for (auto& it : std::ranges::reverse_view(tempDeck))
	{
		deck.push(it);
	}
}

sf::Vector2i boardGameMovable::getMapPosition() const
{
	return mapPosition;
}

void boardGameMovable::setMapPosition(const sf::Vector2i& position)
{
	mapPosition = position;
}

int Player::getExperience() const
{
	return experience;
}

void Player::addExperience(const int value)
{
	experience += value;
}