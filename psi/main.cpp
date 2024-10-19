#include <SFML/Graphics.hpp>
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

    //SFML audio buffer
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("src/APD.wav"))
    {
        //error handling
        return -1;
    }

    //SFML audio player
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setVolume(10);
    sound.play();

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



        }

        window.clear();
        window.display();
    }

    return 0;
}