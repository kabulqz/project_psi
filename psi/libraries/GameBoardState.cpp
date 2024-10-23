#include "GameBoardState.hpp"
#include "Game.hpp"

GameBoardState::GameBoardState(Game* game) : game(game)
{

}

//handler for specific windows to appear in the main frame 
void GameBoardState::handleInput(sf::RenderWindow& window)
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		//if event.type == something => game.changeState(std::make_unique<State>())
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
	//draw elements
	window.display();
}