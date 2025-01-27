#include "GameCardState.hpp"
#include "Game.hpp"

GameCardState::GameCardState(Game* game) : game(game)
{
	// Load the save and create the player and enemy instances
	save = game->getSave();
	std::cout << color("B2FFD6", "finished loading save\n");
	const auto player = new Hero();
	const auto enemy = new Hero();
	player->getDeck().clear();
	enemy->getDeck().clear();
	std::cout << color("B2FFD6", "finished creating player and enemy\n");

	// copy deck from save to player
	player->copyDeck(save->getDeck());
	std::cout << color("B2FFD6", "finished copying deck to player\n");
	// create for-loop to create enemy deck
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution cardSeedDistribution(std::mt19937::min(), std::mt19937::max());
	std::vector<uint_least32_t> tempEnemyDeck;
	tempEnemyDeck.reserve(30);
	for (int i = 0; i < 30; i++) {
		tempEnemyDeck.push_back(cardSeedDistribution(gen));
	}
	std::cout << color("B2FFD6", "finished creating enemy deck\n");
	// copy deck to enemy
	enemy->copyDeck(tempEnemyDeck);
	std::cout << color("B2FFD6", "finished copying deck to enemy\n");
	// set hero cards enemy and hero by method setOwnerAndEnemy(player, enemy)
	for (const auto card : player->getDeck())
	{
		card->setOwnerAndEnemy(player, enemy);
	}
	std::cout << color("B2FFD6", "finished setting owner and enemy for player cards\n");
	// and the other way around but use setOwnerAndEnemy(enemy, player)
	for (const auto card : enemy->getDeck())
	{
		card->setOwnerAndEnemy(enemy, player);
	}
	std::cout << color("B2FFD6", "finished setting owner and enemy for enemy cards\n");
}

//handler for specific windows to appear in the main frame 
void GameCardState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	if (!soundManager.isSoundPlaying("Ambience_crt"))
	{
		soundManager.playSound("Ambience_crt");
	}

	while (eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
<<<<<<< Updated upstream
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
=======
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C)
		{// GO TO CARD GAME
			save->write();
			game->setSave(save);

			game->changeState(std::make_unique<TransitionState>(game, GAME_CARD, GAME_BOARD));
			break;
		}
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) // RELOAD BACKGROUND
		{
			std::random_device rd;
			std::uniform_int_distribution<int> backgroundDistribution(0, 19);
			const int choice = backgroundDistribution(rd);
			chosenBackground = backgrounds[choice];
			std::cout << color("B2FFD6", "chose background number " + std::to_string(choice + 1) + "\n");
		}
		if (cardButton->isClickable() && cardButton->isHovered(mousePos))
		{
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
			{
				//cardButton->getCard()->flip();
				cardButton->startDraggingCard(mousePos);
			}
			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
			{
				//cardButton->getCard()->flip();
				cardButton->stopDraggingCard(mousePos);
			}
		}
		if (PASS.isClickable() && PASS.isHovered(mousePos))
		{
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
			{
				// Pass the turn
				// logic for passing the turn
				// simulating the enemy turn
			}
		}
>>>>>>> Stashed changes
	}
}

//updater for elements corresponding to specific screen
void GameCardState::update()
{

}

//function rendering screen
void GameCardState::render(sf::RenderWindow& window)
{
	window.clear();
	//draw elements
#ifdef _DEBUG

#else

#endif
	window.display();
}

void GameCardState::renderToTexture(sf::RenderTexture& texture)
{
	// Clear the render texture
	texture.clear();
	// Draw the elements
	texture.display();
}
