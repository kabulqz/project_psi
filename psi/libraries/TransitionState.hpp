#pragma once

#include "Settings.hpp"
#include "State.hpp"

enum State_enum
{
	MAIN_MENU,
	GAME_BOARD,
	GAME_CARD,
	ABILITY_TREE
};

class TransitionState : public State
{
	Game* game;
	State_enum targetState;
	std::unique_ptr<State> previousState;

	sf::Cursor defaultCursor;
	sf::Cursor loadingCursor;

	sf::RenderTexture vhsTextureFrom, vhsTextureTo;
	sf::RenderTexture renderTexture, fromTexture, toTexture;
	sf::Shader vhsShader;
	sf::Shader transitionShader;
	sf::Clock transitionClock;
public:
	TransitionState(Game* game, State_enum targetState, std::unique_ptr<State> previousState);
	void handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database) override;
	void update() override;
	void render(sf::RenderWindow& window) override;
	void renderToTexture(sf::RenderTexture& texture) override;
};