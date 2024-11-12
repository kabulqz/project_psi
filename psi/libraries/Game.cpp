#include "Game.hpp"

Game::Game() : window(sf::VideoMode(1280, 720), "Project_PSI", sf::Style::Close),
soundManager(settings.general_audio, settings.ui_audio, settings.environment_audio, settings.alert_audio, settings.music_audio)
{
	//Open Game in MainMenuState
	currentState = std::make_unique<MainMenuState>(this);
	window.setFramerateLimit(60);
	//SFML window icon
	sf::Image icon;
	if (!icon.loadFromFile("src/img/icon.png"))
	{
		exit(-1);
	}
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
}

//function to change between states
void Game::changeState(std::unique_ptr<State> newState)
{
	currentState = std::move(newState);
}

//main function
int Game::run()
{
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	//initializing settings
	//loading database
	//loading sounds
	settings.initialize();
	soundManager.loadSounds();

	//database
	int exit = sqlite3_open("test.db", &settings.database);

	if (exit)
	{
		std::cerr << "Error opening SQLite database\n" << sqlite3_errmsg(settings.database);
		return exit;
	}

	std::cout << "Opened SQLite database successfully\n";
	sqlite3_close(settings.database);

	//SFML cursor & cursor style
	sf::Image cursorImage;
	if (cursorImage.loadFromFile("src/img/cursor.png"))
	{
		sf::Cursor cursor;
		if (cursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(32, 32), sf::Vector2u(0, 0)))
		{
			window.setMouseCursor(cursor);
		}
	}

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
				std::cout << color("DE3C4B", "The window has been closed\n");
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				std::cout << "mouse button clicked at x: "<<color("048A81", std::to_string(mousePos.x)) <<", y: "<<color("048A81", std::to_string(mousePos.y))<<"\n";
				//click3.ogg
				soundManager.playSound("mouseClick");
			}

			eventManager.pushEvent(event);
		}
		currentState->handleInput(window, eventManager, soundManager);
		currentState->update();
		currentState->render(window);
	}

	return 0;
}