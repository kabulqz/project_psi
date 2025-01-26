#include "Slider.hpp"

Slider::Slider(float x, float y, float width, float height, float minValue, float maxValue)
    : minValue(minValue), maxValue(maxValue), value(minValue), x(x), y(y), isDragging(false) {
    // T³o
    background.setSize(sf::Vector2f(width, height));
    background.setPosition(x, y);
    background.setFillColor(sf::Color::Black);
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(2);

    float borderThickness = background.getOutlineThickness();

    // Uchwyt
    slider.setSize(sf::Vector2f(20, height));
    slider.setFillColor(sf::Color::White);

    // Popraw pozycjê uchwytu uwzglêdniaj¹c borderThickness i szerokoœæ uchwytu
    float initialPosition = x + borderThickness;
    slider.setPosition(initialPosition, y);
}

void Slider::handleInput(sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // SprawdŸ, czy mysz jest nad uchwytem suwaka
    bool isMouseOverSlider = slider.getGlobalBounds().contains(mousePos);

    // Obs³uga klikniêcia mysz¹
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (isMouseOverSlider || isDragging) {
            isDragging = true; // Rozpocznij przeci¹ganie
        }
    }
    else {
        isDragging = false; // Zakoñcz przeci¹ganie
    }

    // Przeci¹ganie suwaka
    if (isDragging) {
        float mouseX = mousePos.x;
        float borderThickness = background.getOutlineThickness();

        // Ogranicz pozycjê myszy do obszaru t³a suwaka (z uwzglêdnieniem obramowania)
        mouseX = std::clamp(mouseX, x + borderThickness, x + background.getSize().x - borderThickness);

        // Ustaw now¹ pozycjê uchwytu
        float sliderWidth = slider.getSize().x;
        float sliderX = mouseX - sliderWidth / 2;

        // Ogranicz pozycjê uchwytu do obszaru t³a (z uwzglêdnieniem obramowania)
        sliderX = std::clamp(sliderX, x + borderThickness, x + background.getSize().x - sliderWidth - borderThickness);

        slider.setPosition(sliderX, y);

        // Oblicz now¹ wartoœæ suwaka
        value = minValue + (maxValue - minValue) * ((mouseX - x - borderThickness) / (background.getSize().x - 2 * borderThickness));
    }
}


void Slider::display(sf::RenderTexture& window) {
    window.draw(background);
    window.draw(slider);
}

float Slider::getValue() const {
    return value;
}