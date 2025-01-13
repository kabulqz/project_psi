#pragma once

#include "State.hpp"
#include "TileMap.hpp"
#include "Game.hpp"

class GameBoardState : public State
{
private:
	Game* game;
	Save* save;
	BoardGamePlayer* player;
	std::vector<BoardGameEnemy> boardEnemies;
	TileMap map;

	sf::Vector2i mousePos;
	Button currentLevel;
	Button availableUpgrade;
	Button requiredXP;
	Button statsButton;
	Button playerStats;

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

	GameBoardState(Game* game);
};