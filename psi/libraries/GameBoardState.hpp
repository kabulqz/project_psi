#pragma once

#include "State.hpp"
#include "TileMap.hpp"
#include "Game.hpp"

class GameBoardState : public State
{
private:
	Game* game;
	TileMap map;
	Save save;
	std::vector<sf::Vector2i> path;
public:
	//handler for specific windows to appear in the main frame 
	void handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database) override;
	//updater for elements corresponding to specific screen
	void update() override;
	//function rendering screen
	void render(sf::RenderWindow& window) override;

	GameBoardState(Game* game);
};