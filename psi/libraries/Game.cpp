#include "Game.hpp"
#include <gl/GL.h>

Game::Game() : soundManager(settings.general_audio, settings.ui_audio, settings.ambience_audio, settings.alert_audio, settings.music_audio), save(nullptr)
{
	// Check if we are in DEBUG or RELEASE configuration
#ifdef _DEBUG
	// If in DEBUG mode, use a normal window (not fullscreen)
	window.create(sf::VideoMode(1280, 720), "Project_PSI", sf::Style::Close);
#else
	// If in RELEASE mode, create a fullscreen window
	window.create(sf::VideoMode(1280, 720), "Project_PSI", sf::Style::Fullscreen);
#endif

	window.setActive(true);
	//Open Game in MainMenuState
	currentState = std::make_unique<MainMenuState>(this);
	window.setFramerateLimit(60);
	view.reset(sf::FloatRect(0.f, 0.f, 1280.f, 720.f));

	const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	std::cout << "OpenGL version: " << version << "\n";

	//SFML window icon
	sf::Image icon;
	if (!icon.loadFromFile("src/img/icon.png")) return;
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	//SFML cursor & cursor style
	sf::Image cursorImage;
	if (!cursorImage.loadFromFile("src/img/cursors/cursor_default.png")) return;
	sf::Cursor cursor;
	if (!cursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(32, 32), sf::Vector2u(0, 0))) return;
	window.setMouseCursor(cursor);

	settings.initialize();
	soundManager.loadSounds();

	soundManager.playSound("Intro");
	soundManager.playSound("Ambience_crt");
	gameClock.restart();
}

//function to change between states
void Game::changeState(std::unique_ptr<State> newState)
{
	currentState = std::move(newState);
}

//main function
int Game::run()
{
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
				std::cout << gradient("641220", "e01e37", "closed window by exit to desktop button\n");
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				std::cout << "mouse button clicked at x: "<<color("048A81", std::to_string(mousePos.x)) <<", y: "<<color("048A81", std::to_string(mousePos.y))<<"\n";
				//click3.ogg
				soundManager.playSound("MouseClick");
			}

			eventManager.pushEvent(event);
		}
		currentState->handleInput(window, eventManager, soundManager, settings.database);
		currentState->update();
		currentState->render(window);
	}

	settings.closeDB();
#ifdef _DEBUG
	sf::Time elapsed = gameClock.getElapsedTime();

	// Convert to seconds, minutes, or hours
	float seconds = elapsed.asSeconds();
	float minutes = seconds / 60.0f;
	float hours = minutes / 60.0f;

	int finalHours = static_cast<int>(hours);
	int finalMinutes = static_cast<int>(minutes) % 60;
	int finalSeconds = static_cast<int>(seconds) % 60;

	// Display elapsed time
	std::cout << std::dec << "Game window was open for: ";

	if (finalHours != 0)
	{
		std::cout << finalHours;
		if (finalHours == 1) {
			std::cout << " hour, ";
		}
		else {
			std::cout << " hours, ";
		}
	}
	if (finalMinutes != 0)
	{
		std::cout << finalMinutes;
		if (finalMinutes == 1) {
			std::cout << " minute, ";
		}
		else {
			std::cout << " minutes, ";
		}
	}
	if (finalSeconds != 0)
	{
		std::cout << finalSeconds;
		if (finalSeconds == 1) {
			std::cout << " second\n";
		}
		else {
			std::cout << " seconds\n";
		}
	}
#endif
	return 0;
}