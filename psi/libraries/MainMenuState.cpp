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
sav8Button(895, 505, 330, 130, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
volumeSlider(545, 75, 500, 50, 0.0f, 100.0f),
sliderButton(1065, 75, 150, 50, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
volumeSlider0(545, 175, 500, 50, 0.0f, 100.0f),
sliderButton0(1065, 175, 150, 50, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
volumeSlider1(545, 275, 500, 50, 0.0f, 100.0f),
sliderButton1(1065, 275, 150, 50, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
volumeSlider2(545, 375, 500, 50, 0.0f, 100.0f),
sliderButton2(1065, 375, 150, 50, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
volumeSlider3(545, 475, 500, 50, 0.0f, 100.0f),
sliderButton3(1065, 475, 150, 50, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
saveoptionsButton(965, 535, 50, 50, PATH_TO_BORDERS_FOLDER + "panel-border-019.png"),
readabilityButton(1065, 535, 50, 50, PATH_TO_BORDERS_FOLDER + "panel-border-019.png")
{
	bool hasSaves = this->hasSaves();
	this->options = Options::LOGO;
	continueButton.setText("Continue", game->getSettings().getFont(), game->getSettings().getFontSize());
	continueButton.setEnabled(hasSaves);
	newGameButton.setText("New Game", game->getSettings().getFont(), game->getSettings().getFontSize());
	newGameButton.setEnabled(true);
	loadGameButton.setText("Load Game", game->getSettings().getFont(), game->getSettings().getFontSize());
	loadGameButton.setEnabled(hasSaves);
	settingsButton.setText("Settings", game->getSettings().getFont(), game->getSettings().getFontSize());
	settingsButton.setEnabled(true);
	exitToDesktopButton.setText("Exit", game->getSettings().getFont(), game->getSettings().getFontSize());
	exitToDesktopButton.setEnabled(true);
	
	sav1Button.setText("save 1", game->getSettings().getFont(), game->getSettings().getFontSize() + 2);
	sav2Button.setText("save 2", game->getSettings().getFont(), game->getSettings().getFontSize() + 2);
	sav3Button.setText("save 3", game->getSettings().getFont(), game->getSettings().getFontSize() + 2);
	sav4Button.setText("save 4", game->getSettings().getFont(), game->getSettings().getFontSize() + 2);
	sav5Button.setText("save 5", game->getSettings().getFont(), game->getSettings().getFontSize() + 2);
	sav6Button.setText("save 6", game->getSettings().getFont(), game->getSettings().getFontSize() + 2);
	sav7Button.setText("save 7", game->getSettings().getFont(), game->getSettings().getFontSize() + 2);
	sav8Button.setText("save 8", game->getSettings().getFont(), game->getSettings().getFontSize() + 2);
	sliderButton.setText("Opcje-Dzwiek", game->getSettings().getFont(), game->getSettings().getFontSize() - 8);
	sliderButton0.setText("Opcje-Dzwiek", game->getSettings().getFont(), game->getSettings().getFontSize() - 8);
	sliderButton1.setText("Opcje-Dzwiek", game->getSettings().getFont(), game->getSettings().getFontSize() - 8);
	sliderButton2.setText("Opcje-Dzwiek", game->getSettings().getFont(), game->getSettings().getFontSize() - 8);
	sliderButton3.setText("Opcje-Dzwiek", game->getSettings().getFont(), game->getSettings().getFontSize() - 8);
	saveoptionsButton.setText("save", game->getSettings().getFont(), game->getSettings().getFontSize() - 8);
	readabilityButton.setText("X", game->getSettings().getFont(), game->getSettings().getFontSize() - 8);
	for (int i = 0; i < 8; i++) {
		if (saveArr[i] == 1) {
			saveButtons[i]->setEnabled(true);
		}
		else {
			saveButtons[i]->setEnabled(false);
		}
	}

	if (!vhsShader.loadFromFile("libraries/vhs_effect.frag", sf::Shader::Fragment)) return;
	shaderClock.restart();
}


//Handler for specific windows to appear in the main frame 
void MainMenuState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database) {
	if (!soundManager.isSoundPlaying("Ambience_crt")) {
		soundManager.playSound("Ambience_crt");
	}

	mousePos = sf::Mouse::getPosition(window);

	// Obs³uga suwaków
	volumeSlider.handleInput(window);
	volumeSlider0.handleInput(window);
	volumeSlider1.handleInput(window);
	volumeSlider2.handleInput(window);
	volumeSlider3.handleInput(window);

	// Process events from the event manager
	while (eventManager.hasEvents()) {
		sf::Event event = eventManager.popEvent();
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			// Obs³uga przycisku saveoptionsButton
			if (saveoptionsButton.isHovered(mousePos) && saveoptionsButton.isClickable()) {
				// Pobierz wartoœci z suwaków
				float generalVolume = volumeSlider.getValue();
				float uiVolume = volumeSlider0.getValue();
				float alertVolume = volumeSlider1.getValue();
				float ambienceVolume = volumeSlider2.getValue();
				float musicVolume = volumeSlider3.getValue();

				// Zapisz wartoœci do bazy danych
				saveSoundSettingsToDatabase(database, generalVolume, uiVolume, alertVolume, ambienceVolume, musicVolume);
				game->changeState(std::make_unique<TransitionState>(game, MAIN_MENU, MAIN_MENU));
				std::cout << "Sound settings saved to database.\n";
			}

			// Obs³uga przycisku readabilityButton
			if (readabilityButton.isHovered(mousePos) && readabilityButton.isClickable()) {
				bool currentReadabilityMode = !readabilityMode;
				std::cout << readabilityMode;
				readabilityMode = currentReadabilityMode;
				game->changeState(std::make_unique<TransitionState>(game, MAIN_MENU, MAIN_MENU));
			}

			// Obs³uga przycisku continueButton
			if (continueButton.isHovered(mousePos) && continueButton.isClickable()) {
				int mostRecentSave = MainMenuState::getMostRecentSaveSlot();
				if (mostRecentSave != -1) {
					Save* save = new Save();
					save->load(mostRecentSave);
					this->game->setSave(save);
					game->changeState(std::make_unique<TransitionState>(game, MAIN_MENU, GAME_BOARD));
					soundManager.playSound("Continue");
					soundManager.playSound("Transition");
				}
				else {
					std::cerr << "No valid save files to continue from\n";
				}
			}

			// Obs³uga przycisku newGameButton
			else if (newGameButton.isHovered(mousePos) && newGameButton.isClickable()) {
				if (options != Options::SAVES_WRITE) {
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "SAVES\n");
					options = Options::SAVES_WRITE;

					for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++) {
						if (saveArr[i] == 0 && !saveButtons[i]->isEnabled()) {
							saveButtons[i]->setEnabled(true);
						}
					}
				}
				else {
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "LOGO\n");
					options = Options::LOGO;

					for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++) {
						if (saveArr[i] == 0 && saveButtons[i]->isEnabled()) {
							saveButtons[i]->setEnabled(false);
						}
					}
				}
			}

			// Obs³uga przycisku loadGameButton
			else if (loadGameButton.isHovered(mousePos) && loadGameButton.isClickable()) {
				if (options != Options::SAVES_LOAD) {
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "SAVES\n");
					options = Options::SAVES_LOAD;

					for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++) {
						if (saveArr[i] == 0 && saveButtons[i]->isEnabled()) {
							saveButtons[i]->setEnabled(false);
						}
					}
				}
				else {
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "LOGO\n");
					options = Options::LOGO;
				}
			}

			// Obs³uga przycisku settingsButton
			else if (settingsButton.isHovered(mousePos) && settingsButton.isClickable()) {
				if (options != Options::SETTINGS) {
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "SETTINGS\n");
					options = Options::SETTINGS;

					for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++) {
						if (saveArr[i] == 0 && saveButtons[i]->isEnabled()) {
							saveButtons[i]->setEnabled(false);
						}
					}
				}
				else {
					std::cout << "changed loadingWindow screen to " << color("F4D35E", "LOGO\n");
					options = Options::LOGO;
				}
			}

			// Obs³uga przycisku exitToDesktopButton
			else if (exitToDesktopButton.isHovered(mousePos) && exitToDesktopButton.isClickable()) {
				std::cout << gradient("641220", "e01e37", "closed window by exit to desktop button\n");
				window.close();
			}

			// Obs³uga przycisków zapisów
			else {
				for (int i = 0; i < MAX_NUMBER_OF_SAVES; i++) {
					if (options == Options::SAVES_WRITE) {
						if (saveButtons[i]->isHovered(mousePos) && saveButtons[i]->isClickable()) {
							// Tworzenie nowego zapisu
							Save* save = new Save();
							save->write(i + 1);
							game->setSave(save);
							game->changeState(std::make_unique<TransitionState>(game, MAIN_MENU, GAME_BOARD));
							soundManager.playSound("Continue");
							soundManager.playSound("Transition");
							std::cout << color("3993DD", "created new save on slot " + std::to_string(i + 1) + "\n");
						}
						else if (saveButtons[i]->isHovered(mousePos) && saveButtons[i]->isClickable()) {
							// Nadpisywanie istniej¹cego zapisu
							Save* save = new Save();
							save->write(i + 1);
							game->setSave(save);
							game->changeState(std::make_unique<TransitionState>(game, MAIN_MENU, GAME_BOARD));
							soundManager.playSound("Continue");
							soundManager.playSound("Transition");
							std::cout << color("3993DD", "overwritten save nr " + std::to_string(i + 1) + "\n");
						}
					}
					else if (options == Options::SAVES_LOAD) {
						if (saveArr[i] == 1) {
							if (saveButtons[i]->isHovered(mousePos) && saveButtons[i]->isClickable()) {
								// £adowanie zapisu
								Save* save = new Save();
								save->load(i + 1);
								this->game->setSave(save);
								game->changeState(std::make_unique<TransitionState>(game, MAIN_MENU, GAME_BOARD));
								soundManager.playSound("Continue");
								soundManager.playSound("Transition");
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

	float elapsedTime = shaderClock.getElapsedTime().asSeconds();
	vhsShader.setUniform("time", elapsedTime);
	vhsShader.setUniform("resolution", sf::Vector2f(1280, 720));
}

//Function rendering screen
void MainMenuState::render(sf::RenderWindow& window) {
	// Tworzenie renderTexture do rysowania wszystkich elementów
	sf::RenderTexture renderTexture;
	if (!renderTexture.create(window.getSize().x, window.getSize().y)) {
		std::cerr << "Failed to create render texture\n";
		return;
	}

	// Czyszczenie renderTexture
	renderTexture.clear();

	// Rysowanie przycisków g³ównego menu
	continueButton.display(renderTexture);
	newGameButton.display(renderTexture);
	loadGameButton.display(renderTexture);
	settingsButton.display(renderTexture);
	exitToDesktopButton.display(renderTexture);


	// Rysowanie okna ³adowania (jeœli jest widoczne)
	if (loadWindow.isVisible()) {
		loadWindow.display(renderTexture);

		// Rysowanie zawartoœci okna ³adowania w zale¿noœci od aktualnej opcji
		if (options == Options::LOGO) {
			// Rysowanie logo
			sf::Texture logoTx;
			if (!logoTx.loadFromFile("src/img/icon.png")) {
				std::cerr << "Cannot load logo image\n";
				return;
			}

			sf::Sprite logoSp;
			logoSp.setTexture(logoTx);
			logoSp.setScale(2.0f, 2.0f);

			// Pozycjonowanie logo na œrodku okna ³adowania
			sf::FloatRect loadWindowBounds = loadWindow.getBounds();
			sf::FloatRect logoBounds = logoSp.getLocalBounds();

			float logoX = loadWindowBounds.left + (loadWindowBounds.width - logoBounds.width * logoSp.getScale().x) / 2.0f;
			float logoY = loadWindowBounds.top + (loadWindowBounds.height - logoBounds.height * logoSp.getScale().y) / 2.0f;

			logoSp.setPosition(logoX, logoY);
			renderTexture.draw(logoSp);
		}
		else if (options == Options::SAVES_LOAD || options == Options::SAVES_WRITE) {
			// Rysowanie przycisków zapisów
			sav1Button.display(renderTexture);
			sav2Button.display(renderTexture);
			sav3Button.display(renderTexture);
			sav4Button.display(renderTexture);
			sav5Button.display(renderTexture);
			sav6Button.display(renderTexture);
			sav7Button.display(renderTexture);
			sav8Button.display(renderTexture);
		}
		else if (options == Options::SETTINGS) {
			// Rysowanie dodatkowych elementów w ustawieniach (np. suwak g³oœnoœci)
			volumeSlider.display(renderTexture);
			sliderButton.display(renderTexture);
			volumeSlider0.display(renderTexture);
			sliderButton0.display(renderTexture);
			volumeSlider1.display(renderTexture);
			sliderButton1.display(renderTexture);
			volumeSlider2.display(renderTexture);
			sliderButton2.display(renderTexture);
			volumeSlider3.display(renderTexture);
			sliderButton3.display(renderTexture);
			saveoptionsButton.display(renderTexture);
			readabilityButton.display(renderTexture);
		}
	}

	// Zakoñczenie rysowania na renderTexture
	renderTexture.display();

	// Pobranie sprite'a z renderTexture
	sf::Sprite screenSprite(renderTexture.getTexture());

	// Czyszczenie g³ównego okna
	window.clear();

	// Rysowanie sprite'a na g³ównym oknie (z shaderem lub bez)
#ifdef _DEBUG
	window.draw(screenSprite); // Bez shadera w trybie debugowania
#else
	window.draw(screenSprite, &vhsShader); // Z shaderem w trybie release
#endif

	// Wyœwietlenie zawartoœci okna
	window.display();
}

void MainMenuState::saveSoundSettingsToDatabase(sqlite3*& database, float generalVolume, float uiVolume, float alertVolume, float ambienceVolume, float musicVolume) {
	const char* sql = "REPLACE INTO audio_settings (setting_name, setting_value) VALUES (?, ?);";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(database, sql, -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "Failed to prepare SQL statement: " << sqlite3_errmsg(database) << std::endl;
		return;
	}

	auto saveSetting = [&](const std::string& name, float value) {
		sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_double(stmt, 2, value);
		if (sqlite3_step(stmt) != SQLITE_DONE) {
			std::cerr << "Error saving setting: " << name << " - " << sqlite3_errmsg(database) << std::endl;
		}
		else {
			std::cout << "Saved setting: " << name << " = " << value << "\n";
		}
		sqlite3_reset(stmt);
	};
	float scaledGeneralVolume = generalVolume / 100.0f;
	// Zapisz wartoœci do bazy danych
	saveSetting("general_audio", scaledGeneralVolume);
	saveSetting("ui_audio", uiVolume);
	saveSetting("alert_audio", alertVolume);
	saveSetting("ambience_audio", ambienceVolume);
	saveSetting("music_audio", musicVolume);

	std::cout << "Applied new sound settings.\n";
	sqlite3_finalize(stmt);
	std::cout << "All sound settings saved to the database.\n";
}

void MainMenuState::renderToTexture(sf::RenderTexture& texture)
{
	// Clear the render texture with a background color (if needed)
	texture.clear(sf::Color::Black);

	// Draw all UI elements onto the render texture
	newGameButton.display(texture);
	loadGameButton.display(texture);
	settingsButton.display(texture);
	exitToDesktopButton.display(texture);
	loadWindow.display(texture);

	if (loadWindow.isVisible())
	{
		if (options == Options::LOGO)
		{
			// Render logo
			sf::Texture logoTx;
			if (!logoTx.loadFromFile("src/img/icon.png"))
			{
				std::cerr << "Cannot load logo image\n";
				return;
			}

			sf::Sprite logoSp;
			logoSp.setTexture(logoTx);
			logoSp.setScale(2.0f, 2.0f);

			sf::FloatRect loadWindowBounds = loadWindow.getBounds();
			sf::FloatRect logoBounds = logoSp.getLocalBounds();

			float logoX = loadWindowBounds.left + (loadWindowBounds.width - logoBounds.width * logoSp.getScale().x) / 2.0f;
			float logoY = loadWindowBounds.top + (loadWindowBounds.height - logoBounds.height * logoSp.getScale().y) / 2.0f;

			logoSp.setPosition(logoX, logoY);
			texture.draw(logoSp);
		}
		else if (options == Options::SAVES_LOAD || options == Options::SAVES_WRITE)
		{
			sav1Button.display(texture);
			sav2Button.display(texture);
			sav3Button.display(texture);
			sav4Button.display(texture);
			sav5Button.display(texture);
			sav6Button.display(texture);
			sav7Button.display(texture);
			sav8Button.display(texture);
		}
		else if (options == Options::SETTINGS)
		{
			// Add settings rendering logic here
			volumeSlider.display(texture);
			sliderButton.display(texture);
			volumeSlider0.display(texture);
			sliderButton0.display(texture);
			volumeSlider1.display(texture);
			sliderButton1.display(texture);
			volumeSlider2.display(texture);
			sliderButton2.display(texture);
			volumeSlider3.display(texture);
			sliderButton3.display(texture);
			saveoptionsButton.display(texture);
			readabilityButton.display(texture);
		}
	}

	// Finalize rendering
	texture.display();
}