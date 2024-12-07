#include "GameBoardState.hpp"
#include "MapGeneration.hpp"

//Width and height of a map
const int width = 60;
const int height = 60;

GameBoardState::GameBoardState(Game* game) : game(game)
{
	int level[width * height];

	save = game->getSave();
	std::cout << std::dec << "Level seed: " << save.getSeed() << "\n";

	game->changeViev(0.4f);

	generate(save.getSeed(), level, path);

	//Loading map
	if (!map.load("src/img/test_map_1.png", sf::Vector2u(16, 16), level, width, height))
		return;

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
	}
}

//updater for elements corresponding to specific screen
void GameBoardState::update()
{

}

//function rendering screen
void GameBoardState::render(sf::RenderWindow& window)
{
	window.clear();
	window.setView(game->getView());
	//draw elements
	window.draw(map);
	window.display();
}