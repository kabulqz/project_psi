#pragma once

#include "State.hpp"
#include "Game.hpp"

class GameCardState : public State
{
private:
	Game* game;
	Save* save;

	sf::Texture backgroundTexture;
	sf::Sprite background;

	sf::Shader vhsShader;
	sf::Clock shaderClock;
public:
	//handler for specific windows to appear in the main frame 
	void handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database) override;
	//updater for elements corresponding to specific screen
	void update() override;
	//function rendering screen
	void render(sf::RenderWindow& window) override;
	void renderToTexture(sf::RenderTexture& texture) override;

	GameCardState(Game* game);
};