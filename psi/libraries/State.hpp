#pragma once

#include "Settings.hpp"
#include "Button.hpp"
#include "EventManager.hpp"

class State
{
public:
	//handler for specific windows to appear in the main frame 
	virtual void handleInput(sf::RenderWindow& window, EventManager& eventManager) = 0;
	//updater for elements corresponding to specific screen
	virtual void update() = 0;
	//function rendering screen
	virtual void render(sf::RenderWindow& window) = 0;
	void handleHoverState(Button& button, bool isCurrentlyHovered);
	virtual ~State() = default;
};