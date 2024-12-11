#pragma once

#include "Settings.hpp"
#include "Button.hpp"
#include "EventManager.hpp"
#include "SoundManager.hpp"

class State
{
public:
	//handler for specific windows to appear in the main frame 
	virtual void handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database) = 0;
	//updater for elements corresponding to specific screen
	virtual void update() = 0;
	//function rendering screen
	virtual void render(sf::RenderWindow& window) = 0;
	virtual void renderToTexture(sf::RenderTexture& texture);
	virtual ~State() = default;
};