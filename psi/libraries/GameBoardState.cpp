#include "GameBoardState.hpp"
#include "MapGeneration.hpp"

//Width and height of a map
GameBoardState::GameBoardState(Game* game) : game(game),
currentLevel(10 ,10, 70, 60, PATH_TO_BORDERS_FOLDER + "panel-border-031.png"),
availableUpgrade(85, 20, 40, 40, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
requiredXP(10, 75, 200, 40, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
playerStats(10, 120, 150, 120, PATH_TO_BORDERS_FOLDER + "panel-border-030.png")
{
	srand(static_cast<unsigned>(time(nullptr)));
	save = game->getSave();

	if (save->getPlayer()->getMapPosition() == sf::Vector2i(-1, -1))
	{
		save->getPlayer()->setMapPosition(save->getPath()[0]);
		save->write();
	}
	player = save->getPlayer();

	if (boardEnemies.empty()){
		save->setEnemies(save->getPath());
	}
	boardEnemies = save->getEnemies();

	for (auto& enemy : boardEnemies)
	{
		enemy.setMovementType(static_cast<EntityMovement>(rand() % 2));
	}

	currentLevel.setText(std::to_string(player->getLevel()), game->getSettings().getFont(), game->getSettings().getFontSize() + 4);
	currentLevel.setBackgroundColor("000000");

	availableUpgrade.setVisible(player->hasAvailableAbilityPoints());
	availableUpgrade.setText("+", game->getSettings().getFont(), game->getSettings().getFontSize() + 10);
	availableUpgrade.setBackgroundColor("000000");
	availableUpgrade.setColor("F5B700");

	requiredXP.setText(std::to_string(player->getExperience()) + " / " + std::to_string(player->getTotalXPRequiredForNextLevel()), game->getSettings().getFont(), game->getSettings().getFontSize() - 1);
	requiredXP.setBackgroundColor("000000");
	requiredXP.setVisible(false);

	playerStats.setText("Money: " + std::to_string(player->getMoney()) + "\nHP: " + "\nEnergy: ", game->getSettings().getFont(), game->getSettings().getFontSize() - 1);
	playerStats.setBackgroundColor("000000");
	playerStats.setVisible(false);
	
	game->changeViewZoom(0.4f);

	//Loading map
	if (!map.load("src/img/test_map_1.png", sf::Vector2u(16, 16), save->getLevel(), WIDTH, HEIGHT)) return;

	if (!vhsShader.loadFromFile("libraries/vhs_effect.frag", sf::Shader::Fragment)) return;
	shaderClock.restart();

	if (!redScale.loadFromFile("libraries/redscale.frag", sf::Shader::Fragment)) return;

	sf::Vector2f playerPosition(
		static_cast<float>(player->getMapPosition().x * 16 + 8),
		static_cast<float>(player->getMapPosition().y * 16 + 8));

	sf::View cameraView = game->getView();
	sf::Vector2f cameraViewSize = cameraView.getSize();

	sf::Vector2f mapSize(static_cast<float>(WIDTH * 16), static_cast<float>(HEIGHT * 16));

	sf::Vector2f clampedPosition(
		std::max(cameraViewSize.x / 2.f, std::min(playerPosition.x, mapSize.x - cameraViewSize.x / 2.f)),
		std::max(cameraViewSize.y / 2.f, std::min(playerPosition.y, mapSize.y - cameraViewSize.y / 2.f)));

	cameraView.setCenter(clampedPosition);
	game->setView(cameraView);
}

//handler for specific windows to appear in the main frame 
void GameBoardState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	if (!soundManager.isSoundPlaying("Ambience_crt"))
	{
		soundManager.playSound("Ambience_crt");
	}

	mousePos = sf::Mouse::getPosition(window);

	while (eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();

		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C)
		{// GO TO CARD GAME
			save->setPlayer(player);
			save->setEnemies(boardEnemies);
			save->write();
			game->setSave(save);

			game->changeState(std::make_unique<TransitionState>(game, GAME_BOARD, GAME_CARD));
			soundManager.playSound("Transition");
		}
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		{
			save->setPlayer(player);
			save->setEnemies(boardEnemies);
			save->write();
			game->setSave(save);

			game->changeState(std::make_unique<TransitionState>(game, GAME_BOARD, MAIN_MENU));
			soundManager.playSound("Transition");
			soundManager.playSound("Continue");
		}
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			if (currentLevel.isHovered(mousePos) && currentLevel.isClickable())
			{
				game->changeState(std::make_unique<TransitionState>(game, GAME_BOARD, ABILITY_TREE));
				soundManager.playSound("Transition");
			}
		}
		if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) || (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right))
		{
			//int move = 1; // For testing purposes
			int move = rand() % 6 + 1; // Random move between 1 and 6
			std::cout << "Player rolled a " << color("ff7aa2", std::to_string(move)) << ", ";

			soundManager.playSound("DiceThrow");

			auto temp = save->getPath();
			int i = 0;

			// Find the player's current position in the path
			while (temp[i] != player->getMapPosition())
			{
				i++;
			}

			// Calculate the new index with wrapping
			int newIndex = (i + move) % temp.size();

			// Update the player's position
			player->setMapPosition(save->getPath()[newIndex]);

			save->setPlayer(player);
			save->setEnemies(boardEnemies);
			save->write();
			game->setSave(save);
		}
	}
}

//updater for elements corresponding to specific screen
void GameBoardState::update()
{
	requiredXP.setVisible(currentLevel.isHovered(mousePos));
	playerStats.setVisible(currentLevel.isHovered(mousePos));
	availableUpgrade.setVisible(player->hasAvailableAbilityPoints());
	availableUpgrade.handleHoverState(mousePos);

	if (availableUpgrade.isHovered(mousePos)) {
		availableUpgrade.setText(std::to_string(save->getPlayer()->getAbilityPoints()), game->getSettings().getFont(), game->getSettings().getFontSize() - 1);
	}
	else {
		availableUpgrade.setText("+", game->getSettings().getFont(), game->getSettings().getFontSize() + 10);
	}

	for (auto& enemy : boardEnemies)
	{
		auto sprite = enemy.getSprite();
		sprite.setPosition(enemy.getMapPosition().x * 16, enemy.getMapPosition().y * 16);
		enemy.setSprite(sprite);
	}

	auto sprite = player->getSprite();
	sprite.setPosition(player->getMapPosition().x * 16, player->getMapPosition().y * 16);
	player->setSprite(sprite);


	sf::Vector2f playerPosition(
		static_cast<float>(player->getMapPosition().x * 16 + 8),
		static_cast<float>(player->getMapPosition().y * 16 + 8));

	sf::View cameraView = game->getView();
	sf::Vector2f cameraViewSize = cameraView.getSize();

	sf::Vector2f mapSize(static_cast<float>(WIDTH * 16), static_cast<float>(HEIGHT * 16));

	sf::Vector2f clampedPosition(
		std::max(cameraViewSize.x / 2.f, std::min(playerPosition.x, mapSize.x - cameraViewSize.x / 2.f)),
		std::max(cameraViewSize.y / 2.f, std::min(playerPosition.y, mapSize.y - cameraViewSize.y / 2.f)));

	cameraView.setCenter(clampedPosition);
	game->setView(cameraView);

	float elapsedTime = shaderClock.getElapsedTime().asSeconds();
	vhsShader.setUniform("time", elapsedTime);
	vhsShader.setUniform("resolution", sf::Vector2f(1280, 720));
}

//function rendering screen
void GameBoardState::render(sf::RenderWindow& window)
{
	sf::RenderTexture renderTexture;
	if (!renderTexture.create(window.getSize().x, window.getSize().y))
	{
		std::cerr << "Cannot create render texture\n";
		return;
	}

	renderTexture.clear();
	renderTexture.setView(game->getView());
	//draw elements
	renderTexture.draw(map);

	for (auto& enemy : boardEnemies)
	{
		// Get the enemy's current position and its path
		sf::Vector2i enemyPos = enemy.getMapPosition();
		std::vector<sf::Vector2i> path = save->getPath();

		// Find the current index of the enemy in the path
		int enemyIndex = -1;
		for (int i = 0; i < path.size(); ++i)
		{
			if (path[i] == enemyPos)
			{
				enemyIndex = i;
				break;
			}
		}

		if (enemyIndex != -1) // Only if the enemy is in the path
		{
			// Highlight the tile under the enemy (current position)
			float tileSize = 16.f;  // Tile size in pixels
			sf::RectangleShape highlightUnder(sf::Vector2f(tileSize, tileSize));
			highlightUnder.setPosition(sf::Vector2f(enemyPos.x * tileSize, enemyPos.y * tileSize));
			highlightUnder.setFillColor(sf::Color(255, 0, 0, 45));  // Semi-transparent red (adjust opacity)
			renderTexture.draw(highlightUnder);

			// Highlight tiles up to +2 ahead and -2 behind the enemy
			for (int offset = -2; offset <= 2; ++offset)
			{
				if (offset == 0) continue;  // Skip the current tile where the enemy is

				int index = std::clamp(enemyIndex + offset, 0, static_cast<int>(path.size()) - 1);
				sf::Vector2i tile = path[index];

				// Draw a subtle highlight for this tile
				sf::RectangleShape highlight(sf::Vector2f(tileSize, tileSize));
				highlight.setPosition(sf::Vector2f(tile.x * tileSize, tile.y * tileSize));
				highlight.setFillColor(sf::Color(255, 0, 0, 45));  // Semi-transparent red (adjust opacity)
				renderTexture.draw(highlight);
			}
		}

		// Draw the enemy sprite on top of the highlighted tiles
		enemy.load("src/img/walk.png");
		renderTexture.draw(enemy.getSprite(), &redScale);
	}


	if (save->getPlayer()->getMapPosition() != sf::Vector2i(-1, -1))
	{
		renderTexture.draw(player->getSprite());
	}

	sf::View UI(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
	renderTexture.setView(UI);

	currentLevel.display(renderTexture);
	availableUpgrade.display(renderTexture);
	requiredXP.display(renderTexture);
	playerStats.display(renderTexture);

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

void GameBoardState::renderToTexture(sf::RenderTexture& texture)
{
	// Clear the render texture
	texture.clear(sf::Color::Black);

	// Set the game view
	texture.setView(game->getView());

	// Draw the map
	texture.draw(map);

	for (auto& enemy : boardEnemies)
	{
		// Get the enemy's current position and its path
		sf::Vector2i enemyPos = enemy.getMapPosition();
		std::vector<sf::Vector2i> path = save->getPath();

		// Find the current index of the enemy in the path
		int enemyIndex = -1;
		for (int i = 0; i < path.size(); ++i)
		{
			if (path[i] == enemyPos)
			{
				enemyIndex = i;
				break;
			}
		}

		if (enemyIndex != -1) // Only if the enemy is in the path
		{
			// Highlight the tile under the enemy (current position)
			float tileSize = 16.f;  // Tile size in pixels
			sf::RectangleShape highlightUnder(sf::Vector2f(tileSize, tileSize));
			highlightUnder.setPosition(sf::Vector2f(enemyPos.x * tileSize, enemyPos.y * tileSize));
			highlightUnder.setFillColor(sf::Color(255, 0, 0, 45));  // Semi-transparent red (adjust opacity)
			texture.draw(highlightUnder);

			// Highlight tiles up to +2 ahead and -2 behind the enemy
			for (int offset = -2; offset <= 2; ++offset)
			{
				if (offset == 0) continue;  // Skip the current tile where the enemy is

				int index = std::clamp(enemyIndex + offset, 0, static_cast<int>(path.size()) - 1);
				sf::Vector2i tile = path[index];

				// Draw a subtle highlight for this tile
				sf::RectangleShape highlight(sf::Vector2f(tileSize, tileSize));
				highlight.setPosition(sf::Vector2f(tile.x * tileSize, tile.y * tileSize));
				highlight.setFillColor(sf::Color(255, 0, 0, 45));  // Semi-transparent red (adjust opacity)
				texture.draw(highlight);
			}
		}

		// Draw the enemy sprite on top of the highlighted tiles
		enemy.load("src/img/walk.png");
		texture.draw(enemy.getSprite(), &redScale);
	}

	// Draw the player sprite if their position is valid
	save->getPlayer()->load("src/img/walk.png");
	if (save->getPlayer()->getMapPosition() != sf::Vector2i(-1, -1))
	{
		texture.draw(player->getSprite());
	}

	sf::View UI(sf::FloatRect(0, 0, texture.getSize().x, texture.getSize().y));
	texture.setView(UI);

	currentLevel.display(texture);
	availableUpgrade.display(texture);

	// Finalize rendering
	texture.display();
}