#include "GameCardState.hpp"
#include "Game.hpp"

GameCardState::GameCardState(Game* game) : game(game)
{

}

//handler for specific windows to appear in the main frame 
void GameCardState::handleInput(sf::RenderWindow& window)
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		//if event.type == something => game.changeState(std::make_unique<State>())
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
	window.display();
}