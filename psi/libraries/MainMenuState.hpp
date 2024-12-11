#pragma once

#include "State.hpp"

constexpr int MAX_NUMBER_OF_SAVES = 8;

class MainMenuState : public State
{
private:
	Game* game;
	sf::Shader vhsShader;
	sf::Clock shaderClock;

	sf::Vector2i mousePos;
	Button continueButton;
	Button newGameButton;
	Button loadGameButton;
	Button settingsButton;
	Button exitToDesktopButton;
	Button loadWindow;
	
	Button sav1Button;
	Button sav2Button;
	Button sav3Button;
	Button sav4Button;
	Button sav5Button;
	Button sav6Button;
	Button sav7Button;
	Button sav8Button;
	
	std::array<Button*, 8> saveButtons = {
		&sav1Button, &sav2Button,
		&sav3Button, &sav4Button,
		&sav5Button, &sav6Button,
		&sav7Button, &sav8Button
	};

	enum class Options
	{
		LOGO,
		SAVES_WRITE,
		SAVES_LOAD,
		SETTINGS
	};

	Options options;

	bool saveArr[MAX_NUMBER_OF_SAVES] = { 0 };
	static std::string formatFileTime(const std::filesystem::file_time_type& fileTime);
	bool hasSaves();
	static int getMostRecentSaveSlot();
public:
	//handler for specific windows to appear in the main frame 
	void handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database) override;
	//updater for elements corresponding to specific screen
	void update() override;
	//function rendering screen
	void render(sf::RenderWindow& window) override;
	void renderToTexture(sf::RenderTexture& texture) override;


	MainMenuState(Game* game);
};