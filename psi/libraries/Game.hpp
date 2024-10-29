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
	//window
	sf::RenderWindow window;
	Settings settings;
public:
	Game() : window(sf::VideoMode(1280, 720), "Project_PSI", sf::Style::Close)
	{
		currentState = std::make_unique<MainMenuState>(this);
	}

	//function to change between states
	void changeState(std::unique_ptr<State> newState);

	//main function
	int run();
};