#include    <iostream>
#include    <SFML/Graphics.hpp>
#include    <sqlite3.h>

using namespace std;

int main()
{
    //database
    sqlite3* DB;
    int exit = sqlite3_open("test.db", &DB);

    if (exit)
    {
        cerr << "Error opening SQLite database" << sqlite3_errmsg(DB) << endl;
        return exit;
    }

    cout << "Opened SQLite database successfully" << endl;
    sqlite3_close(DB);


    //SFML window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Project_PSI");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            
        }

        window.clear();
        window.display();
    }

    return 0;
}