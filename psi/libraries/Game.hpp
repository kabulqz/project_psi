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
	sf::View view;
	
	Save save;
public:
	
	Game();
	//function to change between states
	void changeState(std::unique_ptr<State> newState);
	//main function
	int run();

	Save getSave() { return this->save; }
	void setSave(const Save& save)
	{
		this->save = save;
	}
	sf::View getView() { return this->view; }
	void changeView(float scale)
	{
		view.reset(sf::FloatRect(0.f, 0.f, 1280.f * scale, 720.f * scale));
		//std::cout << "View: " << view.getSize().x << " " << view.getSize().y << "\n";
	}
};