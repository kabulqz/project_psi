#include "Game.hpp"

Game::Game() : window(sf::VideoMode(1280, 720), "Project_PSI", sf::Style::Close)
{
	currentState = std::make_unique<MainMenuState>(this);
}

//function to change between states
void Game::changeState(std::unique_ptr<State> newState)
{
	currentState = std::move(newState);
}

//main function
int Game::run()
{
	//initializing settings
	//loading database 
	settings.initialize();

	//database
	int exit = sqlite3_open("test.db", &settings.database);

	if (exit)
	{
		std::cerr << "Error opening SQLite database\n" << sqlite3_errmsg(settings.database);
		return exit;
	}

	std::cout << "Opened SQLite database successfully\n";
	sqlite3_close(settings.database);


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
	mouseClick.setVolume(settings.general_audio * settings.ui_audio);


	//SFML window main loop
	while (window.isOpen())
	{
		sf::Event event;
		eventManager.clear();
		while (window.pollEvent(event))
		{
			//events that are not unique to game states
			if (event.type == sf::Event::Closed)
			{
				std::cout << "The window has been closed\n";
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				std::cout << "mouse button clicked\n";
				//click3.ogg
				mouseClick.play();
			}

			eventManager.pushEvent(event);
		}
		currentState->handleInput(window, eventManager);
		currentState->update();
		currentState->render(window);
	}

	return 0;
}