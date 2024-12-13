#include "GameBoardState.hpp"
#include "MapGeneration.hpp"

//Width and height of a map
GameBoardState::GameBoardState(Game* game) : game(game)
{
	srand(static_cast<unsigned>(time(nullptr)));
	save = game->getSave();
	//std::cout << std::dec << "Level seed: " << save.getSeed() << "\n";

	game->changeViewZoom(0.4f);

	//Loading map
	if (!map.load("src/img/test_map_1.png", sf::Vector2u(16, 16), save->getLevel(), WIDTH, HEIGHT)) return;

	if (save->getPlayer()->getMapPosition() == sf::Vector2i(-1,-1))
	{
		save->getPlayer()->setMapPosition(save->getPath()[0]);
		save->write();
	}
	player = save->getPlayer();

	if (!vhsShader.loadFromFile("libraries/vhs_effect.frag", sf::Shader::Fragment)) return;
	shaderClock.restart();

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

	while (eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		{
			game->changeState(std::make_unique<TransitionState>(game, GAME_BOARD, ABILITY_TREE));
			soundManager.playSound("Transition");
		}
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
		{
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
			save->write();
			game->setSave(save);
		}
	}
}

//updater for elements corresponding to specific screen
void GameBoardState::update()
{
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

	if (save->getPlayer()->getMapPosition() != sf::Vector2i(-1, -1))
	{
		renderTexture.draw(player->getSprite());
	}
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

	// Draw the player sprite if their position is valid
	save->getPlayer()->load("src/img/walk.png");
	if (save->getPlayer()->getMapPosition() != sf::Vector2i(-1, -1))
	{
		texture.draw(player->getSprite());
	}

	// Finalize rendering
	texture.display();
}