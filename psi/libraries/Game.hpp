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
	EventManager eventManager;
	SoundManager soundManager;
	Settings settings;
	
	Save save;
public:
	
	Game();
	//function to change between states
	void changeState(std::unique_ptr<State> newState);
	//main function
	int run();

	Save getSave() { return save; }
	void setSave(Save& save)
	{
		this->save = save;
	}
};



