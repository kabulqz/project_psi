#pragma once

#include "MainMenuState.hpp"
#include "AbilityTreeState.hpp"
#include "GameBoardState.hpp"
#include "GameCardState.hpp"
#include "PauseState.hpp"

class Game
{
private:
	std::unique_ptr<State> currentState;
	//window
	sf::RenderWindow window;
	//database
	sqlite3* database;
public:
	Game() : window(sf::VideoMode(800, 600), "Project_PSI", sf::Style::Close)
	{
		currentState = std::make_unique<MainMenuState>(this);
	}

	//function to change between states
	void changeState(std::unique_ptr<State> newState)
	{
		currentState = std::move(newState);
	}

	//main function
	int run()
	{
		initializeSettings();

		//database
		int exit = sqlite3_open("test.db", &database);

		if (exit)
		{
			std::cerr << "Error opening SQLite database\n" << sqlite3_errmsg(database);
			return exit;
		}

		std::cout << "Opened SQLite database successfully\n";
		sqlite3_close(database);


		//SFML window icon
		sf::Image icon;
		if (!icon.loadFromFile("src/img/icon.png"))
		{
			return -1;
		}

		window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

		//SFML cursor & cursor style
		sf::Cursor cursor;
		sf::Image cursorImage;
		if (cursorImage.loadFromFile("src/img/cursor.png"))
		{
			if (cursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(32, 32), sf::Vector2u(0, 0)))
			{
				window.setMouseCursor(cursor);
			}
		}

		//SFML audio buffer
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile("src/audio/click3.ogg"))
		{
			//error handling
			return -1;
		}

		//SFML audio player
		sf::Sound mouseClick;
		mouseClick.setBuffer(buffer);
		mouseClick.setVolume(10);


		//SFML window main loop
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					std::cout << "The window has been closed\n";
					window.close();
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					//click3.ogg
					mouseClick.play();
				}
			}

			//window.clear();
			//window.display();
			currentState->handleInput(window);
			currentState->update();
			currentState->render(window);
		}

		return 0;
	}
};