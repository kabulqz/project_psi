#include "MainMenuState.hpp"
#include "Game.hpp"

const std::string PATH_TO_BORDERS_FOLDER = "src/img/borders/";

MainMenuState::MainMenuState(Game* game) : game(game),
buttonNext(300, 100, 200, 100, PATH_TO_BORDERS_FOLDER + "panel-border-019.png")
{}

//handler for specific windows to appear in the main frame 
void MainMenuState::handleInput(sf::RenderWindow& window, EventManager& eventManager)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);

	//check if the button is hovered
	bool isCurrentlyHovered = buttonNext.isHovered(mousePos);
	//use the function to handle hover state
	handleHoverState(buttonNext, isCurrentlyHovered);

	//process events from the event manager
	while(eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
		if(event.type == sf::Event::MouseButtonPressed && buttonNext.isHovered(mousePos))
		{
			game->changeState(std::make_unique<GameBoardState>(game));
			std::cout << "changed state\n";
		}
	}
}

//updater for elements corresponding to specific screen
void MainMenuState::update()
{
	//update appearance on the hover state
	buttonNext.updateAppearance(buttonNext.getHovered(), 255, 0, 100);
}

//function rendering screen
void MainMenuState::render(sf::RenderWindow& window)
{
	window.clear();
	//draw main menu elements
	buttonNext.display(window);
	window.display();
}