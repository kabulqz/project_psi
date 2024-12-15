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
		return;
	}

	Card* card = deck.top();
	deck.pop();

	if (hand.size()<10)
	{

		hand.push_back(card);
		card->setZone(TargetZone::HAND);
	}
}

void Hero::shuffleCardIntoTheDeck(Card* card)
{
	if (!card) return; // For safety

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(0, static_cast<int>(deck.size())); // Uniform distribution to get random position

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
	if (this == nullptr)
	{
		sf::Vector2i temp(-1, -1);
		return temp;
	}
	return this->mapPosition;
}

void boardGameMovable::setMapPosition(const sf::Vector2i& position)
{
	this->mapPosition = position;
}

Player::Player()
{
	experience = 0;
	money = 0;
	setMapPosition(sf::Vector2i(-1, -1));
}

int Player::getExperience() const
{
	return experience;
}

void Player::addExperience(const int value)
{
	experience += value;
}

bool Player::load(const std::string& tileset)
{
	// load the tileset texture
	if (!m_playerTexture.loadFromFile(tileset))
		return false;

	m_playerSprite.setTexture(m_playerTexture);
	m_playerSprite.setTextureRect(sf::IntRect(32, 32, 16, 16));

	m_playerSprite.setPosition(static_cast<float>(getMapPosition().x * 16), static_cast<float>(getMapPosition().y * 16));

	return true;
}

std::string Player::serialize() const
{
	std::ostringstream oss;
	oss << getMapPosition().x << "," << getMapPosition().y << "," << experience << "," << money << ",";
	//Add serialization for other atributes here
	return oss.str();
}

Player* Player::deserialize(const std::string& data)
{
	Player* temp = new Player();
	std::istringstream iss(data);
	std::string token;

	std::getline(iss, token, ',');
	int x = std::stoi(token);
	std::getline(iss, token, ',');
	int y = std::stoi(token);
	temp->setMapPosition(sf::Vector2i(x, y));

	std::getline(iss, token, ',');
	temp->addExperience(std::stoi(token));

	std::getline(iss, token, ',');
	temp->addMoney(std::stoi(token));

	return temp;
}