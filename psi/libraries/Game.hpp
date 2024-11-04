#pragma once

#include "Settings.hpp"
#include "MainMenuState.hpp"
#include "AbilityTreeState.hpp"
#include "GameBoardState.hpp"
#include "GameCardState.hpp"


class Game
{
private:
	std::unique_ptr<State> currentState;
	sf::RenderWindow window;
	Settings settings;
	EventManager eventManager;
	SoundManager soundManager;
public:
	Game();
	//function to change between states
	void changeState(std::unique_ptr<State> newState);
	//main function
	int run();
};