#include "GameBoardState.hpp"
#include "MapGeneration.hpp"

//Width and height of a map
constexpr int width = 60;
constexpr int height = 60;

GameBoardState::GameBoardState(Game* game) : game(game)//, circle(4)
{
	int level[width * height];

	save = game->getSave();
	std::cout << std::dec << "Level seed: " << save.getSeed() << "\n";

	game->changeView(0.4f);

	generate(save.getSeed(), level, path);
	std::cout << path[0].x << " " << path[0].y<<"\n";

	//Loading map
	if (!map.load("src/img/test_map_1.png", sf::Vector2u(16, 16), level, width, height))
		return;

	if (save.getPlayer()->getMapPosition() == sf::Vector2i(-1,-1))
	{
		save.getPlayer()->setMapPosition(path[0]);
		save.write();
	}
	player = save.getPlayer();

	if(!save.getPlayer()->load("src/img/walk.png"))
		return;
	//circle.setFillColor(sf::Color::Red);
}

//handler for specific windows to appear in the main frame 
void GameBoardState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	while (eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		{
			game->changeState(std::make_unique<AbilityTreeState>(game));
		}
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
		{
			srand(time(nullptr));
			int move = rand() % 6 + 1; // Random move between 1 and 6
			std::cout << "Spacebar was pressed! " << move << "\n";

			auto temp = path;
			int i = 0;

			// Find the player's current position in the path
			while (temp[i] != player->getMapPosition())
			{
				i++;
			}

			// Calculate the new index with wrapping
			int newIndex = (i + move) % temp.size();

			// Update the player's position
			player->setMapPosition(path[newIndex]);

			save.setPlayer(player);
			save.write();

			std::cout << "player position after " << player->getMapPosition().x << " " << player->getMapPosition().y << "\n";
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

	sf::Vector2f mapSize(static_cast<float>(width * 16), static_cast<float>(height * 16));

	sf::Vector2f clampedPosition(
		std::max(cameraViewSize.x / 2.f, std::min(playerPosition.x, mapSize.x - cameraViewSize.x / 2.f)),
		std::max(cameraViewSize.y / 2.f, std::min(playerPosition.y, mapSize.y - cameraViewSize.y / 2.f)));

	cameraView.setCenter(clampedPosition);
	game->setView(cameraView);
}

//function rendering screen
void GameBoardState::render(sf::RenderWindow& window)
{
	window.clear();
	window.setView(game->getView());
	//draw elements
	window.draw(map);

	/*
	for (const auto& point : path)
	{
		circle.setPosition(static_cast<float>(point.x * 16 + 4), static_cast<float>(point.y * 16 + 4));
		window.draw(circle);
	}
	*/

	if (save.getPlayer()->getMapPosition() != sf::Vector2i(-1, -1))
	{
		window.draw(save.getPlayer()->getSprite());
	}
	window.display();
}