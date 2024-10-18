#include<iostream>
#include<SFML/Graphics.hpp>

using namespace std;

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "Project_PSI");

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