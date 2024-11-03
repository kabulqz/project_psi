#pragma once

#include "State.hpp"

class MainMenuState : public State
{
private:
	Game* game;
	Button testButton;
public:
	//handler for specific windows to appear in the main frame 
	void handleInput(sf::RenderWindow& window, EventManager& eventManager) override;
	//updater for elements corresponding to specific screen
	void update() override;
	//function rendering screen
	void render(sf::RenderWindow& window) override;

	MainMenuState(Game* game);
};