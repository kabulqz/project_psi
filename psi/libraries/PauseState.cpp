#include "PauseState.hpp"
#include "Game.hpp"

PauseState::PauseState(Game* game) : game(game)
{
	
}

//handler for specific windows to appear in the main frame 
void PauseState::handleInput(sf::RenderWindow& window)
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		//if event.type == something => game.changeState(std::make_unique<State>())
	}
}

//updater for elements corresponding to specific screen
void PauseState::update()
{
	
}

//function rendering screen
void PauseState::render(sf::RenderWindow& window)
{
	window.clear();
	//draw elements
	window.display();
}