#include "Hero.hpp"

#include <ranges>
#include "Card.hpp"

bool BoardGameMovable::load(const std::string& tileset)
{
	// load the tileset texture
	if (!m_entityTexture.loadFromFile(tileset))
		return false;

	m_entitySprite.setTexture(m_entityTexture);
	m_entitySprite.setTextureRect(sf::IntRect(32, 32, 16, 16));

	m_entitySprite.setPosition(static_cast<float>(getMapPosition().x * 16), static_cast<float>(getMapPosition().y * 16));

	return true;
}


void BoardGamePlayer::onLevelUp()
{
	std::cout << gradient("ffb700", "ffea00", "Level up! You are now level ")
		<< color("52b788", std::to_string(level)) << "\n";
	level++;
	abilityPoints += 1;
}

int BoardGamePlayer::getRequiredXPForNextLevel() const
{
	// Level 1: 1000 XP
	// Level 2: 1500 XP
	// Level 3: 2000 XP ...
	return 1000 + (level - 1) * 500;
}

int BoardGamePlayer::getTotalXPRequiredForNextLevel() const
{ // UI method to show the total XP required for the next level
	int total = 0;
	for (int i = 1; i <= level; i++)
	{
		total += 1000 + (i - 1) * 500;
	}
	return total;
}

BoardGamePlayer::BoardGamePlayer()
{
	level = 1;
	abilityPoints = 0;
	experience = 0;
	money = 0;

	load("src/img/walk.png");
	setMapPosition(sf::Vector2i(-1, -1));
}

BoardGamePlayer::BoardGamePlayer(const BoardGamePlayer& player) : BoardGameMovable(player)
{
	level = player.level;
	abilityPoints = player.abilityPoints;
	experience = player.experience;
	money = player.money;

	setEntityTexture(player.getEntityTexture());
	sf::Sprite temp;
	temp.setTexture(player.getEntityTexture());
	temp.setTextureRect(player.getSprite().getTextureRect());
	temp.setPosition(player.getSprite().getPosition());
	setSprite(temp);
}

BoardGamePlayer& BoardGamePlayer::operator=(const BoardGamePlayer& player)
{
	if (this == &player) return *this;
	level = player.level;
	abilityPoints = player.abilityPoints;
	experience = player.experience;
	money = player.money;
	setEntityTexture(player.getEntityTexture());
	sf::Sprite temp;
	temp.setTexture(player.getEntityTexture());
	temp.setTextureRect(player.getSprite().getTextureRect());
	temp.setPosition(player.getSprite().getPosition());
	setSprite(temp);
	return *this;
}

void BoardGamePlayer::addExperience(const int value)
{
	experience += value;
	while (experience >= getRequiredXPForNextLevel())
	{
		experience -= getRequiredXPForNextLevel();
		onLevelUp();
	}
}

bool BoardGamePlayer::hasAvailableAbilityPoints() const
{
	return abilityPoints > 0;
}

void BoardGamePlayer::buyAbility(const int abilityCost)
{
	abilityPoints -= abilityCost;
}

std::string BoardGamePlayer::serialize() const
{
	std::ostringstream oss;
	oss << getMapPosition().x << ","
		<< getMapPosition().y << ","
		<< level << ","
		<< abilityPoints << ","
		<< experience << ","
		<< money << ",";
	//Add serialization for other atributes here
	return oss.str();
}

BoardGamePlayer* BoardGamePlayer::deserialize(const std::string& data)
{
	BoardGamePlayer* temp = new BoardGamePlayer();
	std::istringstream iss(data);
	std::string token;

	std::getline(iss, token, ',');
	int x = std::stoi(token);
	std::getline(iss, token, ',');
	int y = std::stoi(token);
	temp->setMapPosition(sf::Vector2i(x, y));

	std::getline(iss, token, ',');
	temp->setLevel(std::stoi(token));

	std::getline(iss, token, ',');
	temp->setAbilityPoints(std::stoi(token));

	std::getline(iss, token, ',');
	temp->setExperience(std::stoi(token));

	std::getline(iss, token, ',');
	temp->setMoney(std::stoi(token));

	return temp;
}

BoardGameEnemy::BoardGameEnemy()
{
	load("src/img/walk.png");
	setMapPosition(sf::Vector2i(-1, -1));
	movementType = EntityMovement::STATIC;
}

BoardGameEnemy::BoardGameEnemy(const sf::Vector2i position)
{
	load("src/img/walk.png");
	setMapPosition(position);
	this->movementType = EntityMovement::STATIC;
}

BoardGameEnemy::BoardGameEnemy(const BoardGameEnemy& enemy) : BoardGameMovable(enemy)
{
	movementType = enemy.movementType;
	setEntityTexture(enemy.getEntityTexture());
	sf::Sprite temp;
	temp.setTexture(enemy.getEntityTexture());
	temp.setTextureRect(enemy.getSprite().getTextureRect());
	temp.setPosition(enemy.getSprite().getPosition());
	setSprite(temp);
}

BoardGameEnemy& BoardGameEnemy::operator=(const BoardGameEnemy& enemy)
{
	if (this == &enemy) return *this;
	movementType = enemy.movementType;
	setEntityTexture(enemy.getEntityTexture());
	sf::Sprite temp;
	temp.setTexture(enemy.getEntityTexture());
	temp.setTextureRect(enemy.getSprite().getTextureRect());
	temp.setPosition(enemy.getSprite().getPosition());
	setSprite(temp);
	return *this;
}

std::string BoardGameEnemy::serialize() const
{
	std::ostringstream oss;
	oss << getMapPosition().x << ","
		<< getMapPosition().y << ",";
	//Add serialization for other attributes here
	return oss.str();
}

BoardGameEnemy* BoardGameEnemy::deserialize(const std::string& data)
{
	BoardGameEnemy* temp = new BoardGameEnemy();
	std::istringstream iss(data);
	std::string token;
	std::getline(iss, token, ',');
	int x = std::stoi(token);
	std::getline(iss, token, ',');
	int y = std::stoi(token);
	temp->setMapPosition(sf::Vector2i(x, y));
	return temp;
}

Hero::Hero(): currentHealth(0), maxEnergy(0), currentEnergy(0)
{
	// Write checks for the deck, hand and battlefield
	hand.reserve(10);
	battlefield.reserve(5);
}

void Hero::dealDamage(const int value)
{
	currentHealth -= value;
	currentHealth = std::max(0, currentHealth);
	if (currentHealth == 0)
	{
		// Game over
	}
}

void Hero::restoreHealth(const int value)
{
	currentHealth += value;
	currentHealth = std::min(currentHealth, maxHealth);
}

void Hero::drawCard()
{
	// If the deck is empty - draw fatigue, then increment it
	// Else draw a card from the deck and call card-draw() method, then add it to the hand
	if (deck.empty())
	{
		// Draw fatigue
		dealDamage(fatigue);
		++fatigue;
	}
	else
	{
		Card* card = deck.front();
		deck.erase(deck.begin());
		if (hand.size() < 10) {
			card->triggerEffect(EffectTrigger::ON_DRAW);
			hand.push_back(card);
		}
		else {
			card->triggerEffect(EffectTrigger::ON_DISCARD);
			delete card;
		}
	}
}

void Hero::discardCard()
{
	if (!hand.empty()) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> handDistribution(0, static_cast<int>(hand.size() - 1));
		int index = handDistribution(gen);
		Card* card = hand[index];
		card->triggerEffect(EffectTrigger::ON_DISCARD);
		hand.erase(hand.begin() + index);
		delete card;
	}
}

void Hero::shuffleCardIntoTheDeck(Card* card)
{
	if (!deck.empty()) {
		std::random_device rd;	
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> deckDistribution(0, static_cast<int>(deck.size() - 1));
		int index = deckDistribution(gen);
		deck.insert(deck.begin() + index, card);
	}
	else {
		deck.push_back(card);
	}
	card->triggerGameEvent(GameEvent::CARD_SHUFFLED);
}

void Hero::modifyEnergy(int value)
{
	// increase max energy by value
	maxEnergy += value;
	// increase current energy by value
	currentEnergy += value;
	// if current energy is greater than max energy, set current energy to max energy
	currentEnergy = std::min(currentEnergy, maxEnergy);
	// if current energy is less than 0, set current energy to 0
	currentEnergy = std::max(currentEnergy, 0);
}

void Hero::applyEffect(std::unique_ptr<IEffectBehavior> effectBehavior)
{
	activeEffects.push_back(std::move(effectBehavior));
}

void Hero::removeEffect(IEffectBehavior* effectBehavior)
{
	// Remove the effect from the active effects list
	std::erase_if(activeEffects, [effectBehavior](const std::unique_ptr<IEffectBehavior>& effect)
		{
			return effect.get() == effectBehavior;
		});
}