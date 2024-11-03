#include "MainMenuState.hpp"
#include "Game.hpp"

const std::string PATH_TO_BORDERS_FOLDER = "src/img/borders/";

MainMenuState::MainMenuState(Game* game) : game(game),
testButton(100, 100, 300, 125, PATH_TO_BORDERS_FOLDER + "panel-border-019.png")
{
	testButton.setText("test", "src/img/antiquity-print.ttf", 30);
	testButton.setEnabled(true);
	//testButton.setColor("3993DD");
}

//handler for specific windows to appear in the main frame 
void MainMenuState::handleInput(sf::RenderWindow& window, EventManager& eventManager)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);

	//check if the button is hovered
	//use the function to handle hover state
	handleHoverState(testButton, testButton.isHovered(mousePos));

	//process events from the event manager
	while(eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
		if(event.type == sf::Event::MouseButtonPressed && testButton.isHovered(mousePos) && testButton.isEnabled() == true)
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
	//zielony - 00F0B5
	//roz / czerwien - F61067
	//zolty - F4D35E
	//niebieski - 3993DD
	testButton.updateAppearance(testButton.getHovered(), "00F0B5");
	//testButton.updateAppearanceWithBaseColor(testButton.getHovered(), "F61067", "00F0B5");
}

//function rendering screen
void MainMenuState::render(sf::RenderWindow& window)
{
	window.clear();
	//draw main menu elements
	testButton.display(window);
	window.display();
}