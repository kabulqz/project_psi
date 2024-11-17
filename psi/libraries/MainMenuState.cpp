#include "MainMenuState.hpp"
#include "Game.hpp"

const std::string PATH_TO_BORDERS_FOLDER = "src/img/borders/";

MainMenuState::MainMenuState(Game* game) : game(game),
continueButton(20, 20, 250, 100, PATH_TO_BORDERS_FOLDER + "panel-border-027.png"),
newGameButton(20, 140, 250, 100, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
loadGameButton(20, 260, 250, 100, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
settingsButton(20, 380, 250, 100, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
exitToDesktopButton(20, 600, 250, 100, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
loadWindow(500, 20, 760, 680, PATH_TO_BORDERS_FOLDER + "panel-border-025.png")
{
	this->options = Options::LOGO;
	continueButton.setText("Continue", "src/img/antiquity-print.ttf", 20);
	continueButton.setEnabled(true);
	newGameButton.setText("New Game", "src/img/antiquity-print.ttf", 20);
	newGameButton.setEnabled(true);
	loadGameButton.setText("Load Game", "src/img/antiquity-print.ttf", 20);
	loadGameButton.setEnabled(true);
	settingsButton.setText("Settings", "src/img/antiquity-print.ttf", 20);
	settingsButton.setEnabled(true);
	exitToDesktopButton.setText("Exit", "src/img/antiquity-print.ttf", 20);
	exitToDesktopButton.setEnabled(true);
	loadWindow.setEnabled(false);
}

//Handler for specific windows to appear in the main frame 
void MainMenuState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	this->mousePos = sf::Mouse::getPosition(window);
	
	//Process events from the event manager
	while(eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			if (continueButton.isHovered(mousePos) && continueButton.isClickable())
			{
				game->changeState(std::make_unique<GameBoardState>(game));
				soundManager.playSound("Continue");
				std::cout << color("00F0B5", "changed state to Board Game\n");
				std::cout << color("3993DD", "continue last from the save\n");
			}
			else if (newGameButton.isHovered(mousePos) && newGameButton.isClickable())
			{
				game->changeState(std::make_unique<GameBoardState>(game));
				soundManager.playSound("Continue");
				soundManager.playSound("Continue");
				std::cout << color("00F0B5", "changed state to Board Game\n");
				std::cout << color("3993DD", "started new gameplay\n");
			}
			else if (loadGameButton.isHovered(mousePos) && loadGameButton.isClickable())
			{
				if (options != Options::SAVES) // Change loadingWindow to SAVES
				{
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "SAVES\n");
					options = Options::SAVES;
				}
				else //options==SAVES so go back to LOGO
				{
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "LOGO\n");
					options = Options::LOGO;
				}
			}
			else if(settingsButton.isHovered(mousePos) && settingsButton.isClickable())
			{
				if (options != Options::SETTINGS) // Change loadingWindow to SETTINGS
				{
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "SETTINGS\n");
					options = Options::SETTINGS;
				}
				else //options==SETTINGS so go back to LOGO
				{
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "LOGO\n");
					options = Options::LOGO;
				}
			}
			else if (exitToDesktopButton.isHovered(mousePos) && exitToDesktopButton.isClickable())
			{
				window.close();
				std::cout << color("F61067", "closed window by exit to desktop button\n");
			}
		}
	}
}

//Updater for elements corresponding to specific screen
void MainMenuState::update()
{
	//Check if the button is hovered
	//Use the function to handle hover state
	continueButton.handleHoverState(mousePos);
	newGameButton.handleHoverState(mousePos);
	loadGameButton.handleHoverState(mousePos);
	settingsButton.handleHoverState(mousePos);
	exitToDesktopButton.handleHoverState(mousePos);
	//Update appearance on the hover state
	//zielony - 00F0B5
	//roz / czerwien - F61067
	//zolty - F4D35E
	//niebieski - 3993DD
	continueButton.updateAppearance("00F0B5");
	newGameButton.updateAppearance("3993DD");
	loadGameButton.updateAppearance("3993DD");
	settingsButton.updateAppearance("F4D35E");
	//testButton.updateAppearanceWithBaseColor("F61067", "00F0B5");
	exitToDesktopButton.updateAppearance("F61067");

}

//Function rendering screen
void MainMenuState::render(sf::RenderWindow& window)
{
	//Clear the window
	window.clear();
	//Add main menu elements
	continueButton.display(window);
	newGameButton.display(window);
	loadGameButton.display(window);
	settingsButton.display(window);
	exitToDesktopButton.display(window);
	loadWindow.display(window);

	if (loadWindow.isVisible())
	{
		if(options==Options::LOGO)
		{
			//Load the logo texture
			sf::Texture logoTx;
			if (!logoTx.loadFromFile("src/img/icon.png"))
			{
				std::cerr << "Cannot load logo img\n";
				exit(-1);
			}
			//Create the sprite for the logo and set the texture
			sf::Sprite logoSp;
			logoSp.setTexture(logoTx);
			//Scale the logo
			logoSp.setScale(2.0f, 2.0f);
			//Calculate the center position within loadWindow
			sf::FloatRect loadWindowBounds = loadWindow.getBounds();
			sf::FloatRect logoBounds = logoSp.getLocalBounds();

			float logoX = loadWindowBounds.left + (loadWindowBounds.width - logoBounds.width * logoSp.getScale().x) / 2.0f;
			float logoY = loadWindowBounds.top + (loadWindowBounds.height - logoBounds.height * logoSp.getScale().y) / 2.0f;

			//Set the position of the logo sprite
			logoSp.setPosition(logoX, logoY);

			//Draw the logo sprite
			window.draw(logoSp);
		}
		else if(options == Options::SAVES)
		{
			
		}
		else if(options == Options::SETTINGS)
		{
			
		}
	}
	//Draw window with elements
	window.display();
}