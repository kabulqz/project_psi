#pragma once

#include "Settings.hpp"

class State
{
public:
	//handler for specific windows to appear in the main frame 
	virtual void handleInput(sf::RenderWindow& window) = 0;
	//updater for elements corresponding to specific screen
	virtual void update() = 0;
	//function rendering screen
	virtual void render(sf::RenderWindow& window) = 0;
	virtual ~State() = default;
};