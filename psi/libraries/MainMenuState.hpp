#pragma once

#include "State.hpp"

class MainMenuState : public State
{
private:
	Game* game;
	sf::Vector2i mousePos;
	Button continueButton;
	Button newGameButton;
	Button loadGameButton;
	Button settingsButton;
	Button exitToDesktopButton;
	Button loadWindow;
	enum class Options
	{
		LOGO,
		SAVES,
		SETTINGS
	};
	Options options;
public:
	//handler for specific windows to appear in the main frame 
	void handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager) override;
	//updater for elements corresponding to specific screen
	void update() override;
	//function rendering screen
	void render(sf::RenderWindow& window) override;

	MainMenuState(Game* game);
};