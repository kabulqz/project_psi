#pragma once

#include "Settings.hpp"
#include "MainMenuState.hpp"
#include "AbilityTreeState.hpp"
#include "GameBoardState.hpp"
#include "GameCardState.hpp"
#include "Save.hpp"


class Game
{
private:
	std::unique_ptr<State> currentState;
	sf::RenderWindow window;
	Settings settings;
	EventManager eventManager;
	SoundManager soundManager;
	uint_least32_t  seed;
	
public:
	uint_least32_t getSeed() {
		return seed;
	}
	void setSeed(uint_least32_t seed) {
		this->seed = seed;
	}
	
	Game();
	//function to change between states
	void changeState(std::unique_ptr<State> newState);
	//main function
	int run();
	
};