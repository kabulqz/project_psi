#include "AbilityTreeState.hpp"
#include "Game.hpp"

AbilityTreeState::AbilityTreeState(Game* game) : game(game)
{

}

//handler for specific windows to appear in the main frame 
void AbilityTreeState::handleInput(sf::RenderWindow& window)
{
	sf::Event event;

	while (window.pollEvent(event))
	{
		//if event.type == something => game.changeState(std::make_unique<State>())
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