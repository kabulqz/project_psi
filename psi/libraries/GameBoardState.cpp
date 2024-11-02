#include "GameBoardState.hpp"
#include "Game.hpp"

GameBoardState::GameBoardState(Game* game) : game(game)
{

}

//handler for specific windows to appear in the main frame 
void GameBoardState::handleInput(sf::RenderWindow& window, EventManager& eventManager)
{
	while (eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
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