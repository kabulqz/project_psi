#include "MainMenuState.hpp"
#include "Game.hpp"

std::string MainMenuState::formatFileTime(const std::filesystem::file_time_type& fileTime)
{
	auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
		fileTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
	std::time_t timeT = std::chrono::system_clock::to_time_t(sctp);

	std::tm tm;
	if (localtime_s(&tm, &timeT) != 0)
	{
		throw std::runtime_error("Failed to convert time");
	}

	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}

bool MainMenuState::hasSaves()
{
	bool saveIsExisting = false;
	const std::string save_path = std::filesystem::absolute("src/saves/").string();

	for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++)
	{
		std::string filename = "save" + std::to_string(i + 1) + ".sav";
		std::filesystem::path filepath = save_path + filename;
		if (std::filesystem::exists(filepath))
		{
			std::cout << "file \"" + color("AEC5EB", filename) + "\" exists\n";
			try
			{
				auto lastWriteTime = std::filesystem::last_write_time(filepath);
				std::string formattedTime = formatFileTime(lastWriteTime);
				std::cout << "Last modified: " + color("F4D35E", formattedTime) << "\n";
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error retrieving  file time for " << filename << ": " << e.what() << "\n";
			}

			saveArr[i] = true;
		}
	}

	for (const bool i : saveArr)
	{
		if (i == 1) 
		{
			saveIsExisting = true; 
		}
	}

	return saveIsExisting;
}

int MainMenuState::getMostRecentSaveSlot()
{
	const std::string save_path = "src/saves/";
	int mostRecentSlot = -1;
	std::filesystem::file_time_type mostRecentTime = std::filesystem::file_time_type::min();

	for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++)
	{
		std::string filename = save_path + "save" + std::to_string(i + 1) + ".sav";
		if (std::filesystem::exists(filename))
		{
			auto lastWriteTime = std::filesystem::last_write_time(filename);
			if (lastWriteTime > mostRecentTime)
			{
				mostRecentTime = lastWriteTime;
				mostRecentSlot = i+1;
			}
		}
	}

	return mostRecentSlot;
}

const std::string PATH_TO_BORDERS_FOLDER = "src/img/borders/";

MainMenuState::MainMenuState(Game* game) : game(game),
continueButton(20, 20, 250, 100, PATH_TO_BORDERS_FOLDER + "panel-border-027.png"),
newGameButton(20, 140, 250, 100, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
loadGameButton(20, 260, 250, 100, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
settingsButton(20, 380, 250, 100, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
exitToDesktopButton(20, 600, 250, 100, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
loadWindow(500, 20, 760, 680, PATH_TO_BORDERS_FOLDER + "panel-border-025.png"),
sav1Button(545,85,330,130,PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
sav2Button(545, 225, 330, 130, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
sav3Button(545, 365, 330, 130, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
sav4Button(545, 505, 330, 130, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
sav5Button(895, 85, 330, 130, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
sav6Button(895, 225, 330, 130, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
sav7Button(895, 365, 330, 130, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
sav8Button(895, 505, 330, 130, PATH_TO_BORDERS_FOLDER + "panel-border-019.png")
{
	bool hasSaves = this->hasSaves();
	this->options = Options::LOGO;
	continueButton.setText("Continue", "src/img/antiquity-print.ttf", 21);
	continueButton.setEnabled(hasSaves);
	newGameButton.setText("New Game", "src/img/antiquity-print.ttf", 21);
	newGameButton.setEnabled(true);
	loadGameButton.setText("Load Game", "src/img/antiquity-print.ttf", 21);
	loadGameButton.setEnabled(hasSaves);
	settingsButton.setText("Settings", "src/img/antiquity-print.ttf", 21);
	settingsButton.setEnabled(true);
	exitToDesktopButton.setText("Exit", "src/img/antiquity-print.ttf", 21);
	exitToDesktopButton.setEnabled(true);

	sav1Button.setText("save 1", "src/img/antiquity-print.ttf", 23);
	sav2Button.setText("save 2", "src/img/antiquity-print.ttf", 23);
	sav3Button.setText("save 3", "src/img/antiquity-print.ttf", 23);
	sav4Button.setText("save 4", "src/img/antiquity-print.ttf", 23);
	sav5Button.setText("save 5", "src/img/antiquity-print.ttf", 23);
	sav6Button.setText("save 6", "src/img/antiquity-print.ttf", 23);
	sav7Button.setText("save 7", "src/img/antiquity-print.ttf", 23);
	sav8Button.setText("save 8", "src/img/antiquity-print.ttf", 23);

	for (int i = 0; i < 8; i++) {
		if (saveArr[i] == 1) {
			saveButtons[i]->setEnabled(true);
		}
		else {
			saveButtons[i]->setEnabled(false);
		}
	}
}

//Handler for specific windows to appear in the main frame 
void MainMenuState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	mousePos = sf::Mouse::getPosition(window);

	//Process events from the event manager
	while(eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
		{
			if (continueButton.isHovered(mousePos) && continueButton.isClickable())
			{
				int mostRecentSave = MainMenuState::getMostRecentSaveSlot();
				if (mostRecentSave != -1)
				{
					Save save;
					this->game->setSave(save.load(mostRecentSave));
					game->changeState(std::make_unique<GameBoardState>(game));
					soundManager.playSound("Continue");
					std::cout << color("00F0B5", "changed state to Board Game\n");
					std::cout << color("3993DD", "continue from the last save\n");
				}
				else
				{
					std::cerr << "No valid save files to continue from\n";
				}
			}
			else if (newGameButton.isHovered(mousePos) && newGameButton.isClickable())
			{
				if (options != Options::SAVES_WRITE) // Change loadingWindow to SAVES
				{
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "SAVES\n");
					options = Options::SAVES_WRITE;

					for (int i=0;i<MAX_NUMBER_OF_SAVES;i++)
					{
						if (saveArr[i] == 0 && !saveButtons[i]->isEnabled())
						{
							saveButtons[i]->setEnabled(true);
						}
					}
				}
				else //options==SAVES so go back to LOGO
				{
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "LOGO\n");
					options = Options::LOGO;

					for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++)
					{
						if (saveArr[i] == 0 && saveButtons[i]->isEnabled())
						{
							saveButtons[i]->setEnabled(false);
						}
					}
				}
			}
			else if (loadGameButton.isHovered(mousePos) && loadGameButton.isClickable())
			{
				if (options != Options::SAVES_LOAD) // Change loadingWindow to SAVES
				{
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "SAVES\n");
					options = Options::SAVES_LOAD;

					for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++)
					{
						if (saveArr[i] == 0 && saveButtons[i]->isEnabled())
						{
							saveButtons[i]->setEnabled(false);
						}
					}
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

					for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++)
					{
						if (saveArr[i] == 0 && saveButtons[i]->isEnabled())
						{
							saveButtons[i]->setEnabled(false);
						}
					}
				}
				else //options==SETTINGS so go back to LOGO
				{
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "LOGO\n");
					options = Options::LOGO;
				}
			}
			else if (exitToDesktopButton.isHovered(mousePos) && exitToDesktopButton.isClickable())
			{
				std::cout << color("F61067", "closed window by exit to desktop button\n");
				window.close();
			}
			else {
				for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++) {
					if (options == Options::SAVES_WRITE) {
						if (saveButtons[i]->isHovered(mousePos) && saveButtons[i]->isClickable())
						{//new save
							Save save;
							save.write(i + 1);
							game->setSave(save);
							game->changeState(std::make_unique<GameBoardState>(game));
							soundManager.playSound("Continue");
							std::cout << color("00F0B5", "changed state to Board Game\n");
							std::cout << color("3993DD", "created new save on slot " + std::to_string(i + 1) + "\n");
						}
						else if (saveButtons[i]->isHovered(mousePos) && saveButtons[i]->isClickable())
						{//overwrite
							Save save;
							save.write(i + 1);
							game->setSave(save);
							game->changeState(std::make_unique<GameBoardState>(game));
							soundManager.playSound("Continue");
							std::cout << color("00F0B5", "changed state to Board Game\n");
							std::cout << color("3993DD", "overwritten save nr " + std::to_string(i+1) + "\n");
						}
					}
					else if (options == Options::SAVES_LOAD)
					{
						if (saveArr[i] == 1) {
							if (saveButtons[i]->isHovered(mousePos) && saveButtons[i]->isClickable()) {
								Save save;
								this->game->setSave(save.load(i + 1));
								game->changeState(std::make_unique<GameBoardState>(game));
								soundManager.playSound("Continue");
								std::cout << color("00F0B5", "changed state to Board Game\n");
								std::cout << color("3993DD", "continue last from the save\n");
							}
						}
					}
				}
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
	exitToDesktopButton.updateAppearance("F61067");

	for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++)
	{
		saveButtons[i]->handleHoverState(mousePos);
		if (options == Options::SAVES_LOAD)
		{
			saveButtons[i]->updateAppearance("00F0B5");
		}
		else if (options == Options::SAVES_WRITE)
		{
			if (saveArr[i] == true && saveButtons[i]->isEnabled())
			{
				saveButtons[i]->updateAppearanceWithBaseColor("F4D35E", "F61067");
			}
			else
			{
				saveButtons[i]->updateAppearanceWithBaseColor("343A40", "00F0B5");
			}
		}
	}
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
				return;
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
		else if(options == Options::SAVES_LOAD || options == Options::SAVES_WRITE)
		{
			sav1Button.display(window);
			sav2Button.display(window);
			sav3Button.display(window);
			sav4Button.display(window);
			sav5Button.display(window);
			sav6Button.display(window);
			sav7Button.display(window);
			sav8Button.display(window);
		}
		else if(options == Options::SETTINGS)
		{
			
		}
	}
	//Draw window with elements
	window.display();
}