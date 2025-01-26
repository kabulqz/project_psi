#pragma once

#include "State.hpp"
#include "Slider.hpp"
#include "Settings.hpp"

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
	Slider volumeSlider;
	Button sliderButton;
	Button sliderButton0;
	Button sliderButton1;
	Button sliderButton2;
	Button sliderButton3;
	Slider volumeSlider0;
	Slider volumeSlider1;
	Slider volumeSlider2;
	Slider volumeSlider3;
	Button readabilityButton;
	Button saveoptionsButton;
	bool readabilityMode = false;
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
	void saveSoundSettingsToDatabase(sqlite3*& database, float generalVolume, float uiVolume, float alertVolume, float ambienceVolume, float musicVolume);
	//updater for elements corresponding to specific screen
	void update() override;
	//function rendering screen
	void render(sf::RenderWindow& window) override;
	void renderToTexture(sf::RenderTexture& texture) override;

	void toggleReadabilityMode(sqlite3*& database);
	MainMenuState(Game* game);
};