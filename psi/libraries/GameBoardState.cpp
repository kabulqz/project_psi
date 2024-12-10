#include "GameBoardState.hpp"
#include "MapGeneration.hpp"

//Width and height of a map
const int width = 60;
const int height = 60;

GameBoardState::GameBoardState(Game* game) : game(game)//, circle(4)
{
	int level[width * height];

	save = game->getSave();
	std::cout << std::dec << "Level seed: " << save.getSeed() << "\n";

	game->changeView(1.f);

	generate(save.getSeed(), level, path);
	std::cout << path[0].x << " " << path[0].y<<"\n";

	//Loading map
	if (!map.load("src/img/test_map_1.png", sf::Vector2u(16, 16), level, width, height))
		return;

	player = save.getPlayer();
	player.setMapPosition(path[0]);

	if(!player.load("src/img/walk.png"))
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
			srand(time(NULL));
			int move = rand() % 6 + 1;
			std::cout << "Spacebar was pressed! " << move << "\n";
			auto temp = path;
			int i = 0;
			std::cout << "player position before " << player.getMapPosition().x << " " << player.getMapPosition().y << "\n";
			while (temp[i] != player.getMapPosition())
			{
				i++;
			}	
			player.setMapPosition(path[i + move]);
			
			std::cout << "player position after " << player.getMapPosition().x << " " << player.getMapPosition().y << "\n";
		}
	}
}

//updater for elements corresponding to specific screen
void GameBoardState::update()
{
	auto sprite = player.getSprite();
	sprite.setPosition(player.getMapPosition().x * 16, player.getMapPosition().y * 16);
	player.setSprite(sprite);
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

	window.draw(player.getSprite());
	window.display();
}