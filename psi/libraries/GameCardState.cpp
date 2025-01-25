#include "GameCardState.hpp"
#include "Game.hpp"

GameCardState::GameCardState(Game* game) : game(game)
{
	std::random_device rd;
	// Load the save and create the player and enemy instances
	save = game->getSave();
	game->changeViewZoom(1.0f);
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

	std::uniform_int_distribution<int> backgroundDistribution(1, 20);
	const int choice = backgroundDistribution(rd);
	if (!backgroundTexture.loadFromFile("src/img/background/" + std::to_string(choice) + ".png")) {
		std::cerr << "Error: Could not load background image\n";
	}
	else {
		std::cout << color("B2FFD6", "loaded background image, chose background nr " + std::to_string(choice) + "\n");
		background.setTexture(backgroundTexture);
		background.setPosition(0, 0); // Ensure it's positioned correctly
	}

	if (!vhsShader.loadFromFile("libraries/vhs_effect.frag", sf::Shader::Fragment)) return;
	shaderClock.restart();
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
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C)
		{// GO TO CARD GAME
			save->write();
			game->setSave(save);

			game->changeState(std::make_unique<TransitionState>(game, GAME_CARD, GAME_BOARD));
		}

	}
}

//updater for elements corresponding to specific screen
void GameCardState::update()
{
	float elapsedTime = shaderClock.getElapsedTime().asSeconds();
	vhsShader.setUniform("time", elapsedTime);
	vhsShader.setUniform("resolution", sf::Vector2f(1280, 720));
}

//function rendering screen
void GameCardState::render(sf::RenderWindow& window)
{
	sf::RenderTexture renderTexture;
	if (!renderTexture.create(window.getSize().x, window.getSize().y)) {
		std::cerr << "Cannot create render texture\n";
		return;
	}
	renderTexture.clear(sf::Color::Transparent);
	renderTexture.setView(game->getView());

	background.setScale(window.getSize().x / background.getLocalBounds().width,
		window.getSize().y / background.getLocalBounds().height);
	renderTexture.draw(background);

	renderTexture.display();
	window.clear();
	sf::Sprite screenSprite(renderTexture.getTexture());

#ifdef _DEBUG
	window.draw(screenSprite);
#else
	window.draw(screenSprite, &vhsShader);
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
