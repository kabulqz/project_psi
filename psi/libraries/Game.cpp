#include "Game.hpp"
#include <gl/GL.h>

Game::Game() : window(sf::VideoMode(1280, 720), "Project_PSI", sf::Style::Close),
soundManager(settings.general_audio, settings.ui_audio, settings.environment_audio, settings.alert_audio, settings.music_audio)
{
	window.setActive(true);
	//Open Game in MainMenuState
	currentState = std::make_unique<MainMenuState>(this);
	window.setFramerateLimit(60);
	view.reset(sf::FloatRect(0.f, 0.f, 1280.f, 720.f));
	//SFML window icon
	sf::Image icon;
	if (!icon.loadFromFile("src/img/icon.png"))
	{
		return;
	}
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	//SFML cursor & cursor style
	sf::Image cursorImage;
	if (!cursorImage.loadFromFile("src/img/cursor_default.png"))
	{
		return;
	}
	sf::Cursor cursor;
	if (!cursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(32, 32), sf::Vector2u(0, 0)))
	{
		return;
	}
	window.setMouseCursor(cursor);

	const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	std::cout << "OpenGL version: " << version << "\n";
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
		currentState->handleInput(window, eventManager, soundManager, settings.database);
		currentState->update();
		currentState->render(window);
	}

	settings.closeDB();
	return 0;
}