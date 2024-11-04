#include "AbilityTreeState.hpp"
#include "Game.hpp"

AbilityTreeState::AbilityTreeState(Game* game) : game(game)
{

}

//handler for specific windows to appear in the main frame 
void AbilityTreeState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager)
{
	while (eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
	}
}

//updater for elements corresponding to specific screen
void AbilityTreeState::update()
{

}

//function rendering screen
void AbilityTreeState::render(sf::RenderWindow& window)
{
	window.clear();
	//draw elements
	window.display();
}