#include "GameCardState.hpp"
#include "Game.hpp"

GameCardState::GameCardState(Game* game) : game(game),
enemyHand(160, 10, 960, 160, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
playerHand(160, 550, 960, 160, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
enemyBattlefield(140, 175, 1000, 180, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
playerBattlefield(140, 365, 1000, 180, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
enemyDeck(1152, 154, 96, 128, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
playerDeck(1152, 438, 96, 128, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
PASS(10, 320, 120, 80, PATH_TO_BORDERS_FOLDER + "panel-border-031.png")
{
	PASS.setBackgroundColor("000000");
	PASS.setText("PASS", game->getSettings().getFont(), game->getSettings().getFontSize());
	PASS.setEnabled(false);

	std::random_device rd;
	// Load the save and create the player and enemy instances
	save = game->getSave();
	game->changeViewZoom(1.0f);
	player = new Hero();
	enemy = new Hero();
	player->getDeck().clear();
	enemy->getDeck().clear();

	// copy deck from save to player
	player->copyDeck(save->getDeck());
	// create for-loop to create enemy deck
	std::mt19937 gen(rd());
	std::uniform_int_distribution cardSeedDistribution(std::mt19937::min(), std::mt19937::max());
	std::vector<uint_least32_t> tempEnemyDeck;
	tempEnemyDeck.reserve(30);
	for (int i = 0; i < 30; i++) {
		tempEnemyDeck.push_back(cardSeedDistribution(gen));
	}
	// copy deck to enemy
	enemy->copyDeck(tempEnemyDeck);
	// set hero cards enemy and hero by method setOwnerAndEnemy(player, enemy)
	for (const auto card : player->getDeck())
	{
		card->setOwnerAndEnemy(player, enemy);
	}
	// and the other way around but use setOwnerAndEnemy(enemy, player)
	for (const auto card : enemy->getDeck())
	{
		card->setOwnerAndEnemy(enemy, player);
	}

	std::uniform_int_distribution<int> backgroundDistribution(0, 19);
	const int choice = backgroundDistribution(rd);
	chosenBackground = backgrounds[choice];

	if (!backgroundShader.loadFromFile("libraries/background.frag", sf::Shader::Fragment)) return;
	if (!vhsShader.loadFromFile("libraries/vhs_effect.frag", sf::Shader::Fragment)) return;
	shaderClock.restart();

	cardButton = new CardButton(enemy->getDeck().back());
	cardButton->getCard()->setFontAndFontSize(game->getSettings().getFont(), game->getSettings().getFontSize() - 5);
	cardButton->getCard()->flip();
}

//handler for specific windows to appear in the main frame 
void GameCardState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	if (!soundManager.isSoundPlaying("Ambience_crt"))
	{
		soundManager.playSound("Ambience_crt");
	}

	mousePos = sf::Mouse::getPosition(window);

	if (cardButton->isBeingDragged())
	{
		cardButton->updatePosition(mousePos);
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
	}
}

//updater for elements corresponding to specific screen
void GameCardState::update()
{
	float elapsedTime = shaderClock.getElapsedTime().asSeconds();

	// Update VHS shader
	vhsShader.setUniform("time", elapsedTime);
	vhsShader.setUniform("resolution", sf::Vector2f(1280, 720));

	// Update background shader
	backgroundShader.setUniform("time", elapsedTime);
	backgroundShader.setUniform("spin_time", elapsedTime * 0.4f);
	backgroundShader.setUniform("colour_1", chosenBackground.colour1); // Adjust colors as needed
	backgroundShader.setUniform("colour_2", chosenBackground.colour2);
	backgroundShader.setUniform("colour_3", chosenBackground.colour3);
	backgroundShader.setUniform("contrast", 1.0f);
	backgroundShader.setUniform("spin_amount", 0.7f);
	backgroundShader.setUniform("screenSize", sf::Vector2f(1280.0f, 720.0f));
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

	sf::RectangleShape fullScreenQuad(sf::Vector2f(window.getSize().x, window.getSize().y));
	//renderTexture.draw(background);
	renderTexture.draw(fullScreenQuad, &backgroundShader);

	playerHand.display(renderTexture);
	playerBattlefield.display(renderTexture);
	playerDeck.display(renderTexture);
	enemyHand.display(renderTexture);
	enemyBattlefield.display(renderTexture);
	enemyDeck.display(renderTexture);
	PASS.display(renderTexture);

	cardButton->display(renderTexture);

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
	texture.setView(game->getView());

	sf::RectangleShape fullScreenQuad(sf::Vector2f(1280, 720));
	//renderTexture.draw(background);
	backgroundShader.setUniform("time", shaderClock.getElapsedTime().asSeconds());
	backgroundShader.setUniform("spin_time", shaderClock.getElapsedTime().asSeconds() * 0.4f);
	backgroundShader.setUniform("colour_1", chosenBackground.colour1); // Adjust colors as needed
	backgroundShader.setUniform("colour_2", chosenBackground.colour2);
	backgroundShader.setUniform("colour_3", chosenBackground.colour3);
	backgroundShader.setUniform("contrast", 1.0f);
	backgroundShader.setUniform("spin_amount", 0.7f);
	backgroundShader.setUniform("screenSize", sf::Vector2f(1280.0f, 720.0f));
	texture.draw(fullScreenQuad, &backgroundShader);

	playerHand.display(texture);
	playerBattlefield.display(texture);
	playerDeck.display(texture);
	enemyHand.display(texture);
	enemyBattlefield.display(texture);
	enemyDeck.display(texture);
	PASS.display(texture);
	// Draw the elements
	texture.display();
}
