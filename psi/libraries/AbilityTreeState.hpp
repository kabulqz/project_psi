#pragma once

#include "State.hpp"

class AbilityTreeState : public State
{
public:
	//handler for specific windows to appear in the main frame 
	void handleInput(sf::RenderWindow& window) override
	{
	}
	//updater for elements corresponding to specific screen
	void update() override
	{
	}
	//function rendering screen
	void render(sf::RenderWindow& window) override
	{
		window.clear();
		//draw elements
		window.display();
	}
};