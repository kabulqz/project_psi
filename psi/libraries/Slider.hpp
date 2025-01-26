#ifndef SLIDER_HPP
#define SLIDER_HPP

#include <SFML/Graphics.hpp>

class Slider {
public:
    Slider(float x, float y, float width, float height, float minValue, float maxValue);

    void handleInput(sf::RenderWindow& window);
    void display(sf::RenderTexture& window);
    float getValue() const;

private:
    sf::RectangleShape background;
    sf::RectangleShape slider;
    float minValue, maxValue, value;
    float x, y; // Dodajemy y, aby œledziæ pozycjê suwaka
    bool isDragging; // Flaga przeci¹gania
};

#endif // SLIDER_HPP