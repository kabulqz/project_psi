#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <sqlite3.h>
#include <iostream>

using namespace std;

int main()
{
	//database
	sqlite3* DB;
	int exit = sqlite3_open("test.db", &DB);

	if (exit)
	{
		cerr << "Error opening SQLite database\n" << sqlite3_errmsg(DB);
		return exit;
	}

	cout << "Opened SQLite database successfully\n";
	sqlite3_close(DB);


	//SFML window handler
	sf::RenderWindow window(sf::VideoMode(800, 600), "Project_PSI");

	//SFML window icon
	sf::Image icon;
	if (!icon.loadFromFile("src/icon.png"))
	{
		return -1;
	}

	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	//SFML cursor & cursor style
	sf::Cursor cursor;
	sf::Image cursorImage;
	if (cursorImage.loadFromFile("src/cursor.png"))
	{
		if(cursor.loadFromPixels(cursorImage.getPixelsPtr(), sf::Vector2u(16, 16), sf::Vector2u(0, 0)))
		{
			window.setMouseCursor(cursor);
		}
	}

	//SFML audio buffer
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("src/click3.ogg"))
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
				cout << "The window has been closed\n";
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				//click3.ogg
				mouseClick.play();
			}



		}

		window.clear();
		window.display();
	}

	return 0;
}