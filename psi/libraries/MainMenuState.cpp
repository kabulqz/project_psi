#include "MainMenuState.hpp"
#include "Game.hpp"

MainMenuState::MainMenuState(Game* game) : game(game)
{
	
}

//handler for specific windows to appear in the main frame 
void MainMenuState::handleInput(sf::RenderWindow& window)
{
	sf::Event event;

	while(window.pollEvent(event))
	{
		//if event.type == something => game.changeState(std::make_unique<State>())
	}
}

//updater for elements corresponding to specific screen
void MainMenuState::update()
{
	
}

//function rendering screen
void MainMenuState::render(sf::RenderWindow& window)
{
	window.clear();
	//draw main menu elements
	window.display();
}