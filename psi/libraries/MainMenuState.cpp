#include "MainMenuState.hpp"
#include "Game.hpp"

const std::string PATH_TO_BORDERS_FOLDER = "src/img/borders/";

MainMenuState::MainMenuState(Game* game) : game(game),
buttonNext(300, 100, PATH_TO_BORDERS_FOLDER + "panel-border-022.png", 200, 300)
{}

//handler for specific windows to appear in the main frame 
void MainMenuState::handleInput(sf::RenderWindow& window)
{
	sf::Event event;

	while(window.pollEvent(event))
	{
		//if event.type == something => game.changeState(std::make_unique<State>())
	}

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	bool hovered = buttonNext.isHovered(mousePos);

	if(hovered && !buttonNext.hovered)
	{
		buttonNext.hoverClock.restart();
	}
	buttonNext.hovered = hovered;

	buttonNext.updateAppearance(hovered, 255, 0, 100);
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
	buttonNext.display(window);

	window.display();
}