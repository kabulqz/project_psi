#include "State.hpp"

Button::Button(int buttonWidth, int buttonHeight, const std::string& path_to_file, const int& pos_x, const int& pos_y) : width(buttonWidth), height(buttonHeight), position{ pos_x, pos_y }
{
	if (!buttonTexture.loadFromFile(path_to_file))
	{
		std::cout << "Cannot load button img\n"; return;
	}

	//corners
	topLeft.setTexture(buttonTexture);
	topLeft.setTextureRect(sf::IntRect(0, 0, 24, 24));
	topRight.setTexture(buttonTexture);
	topRight.setTextureRect(sf::IntRect(24, 0, 24, 24));
	bottomLeft.setTexture(buttonTexture);
	bottomLeft.setTextureRect(sf::IntRect(0, 24, 24, 24));
	bottomRight.setTexture(buttonTexture);
	bottomRight.setTextureRect(sf::IntRect(24, 24, 24, 24));

	topLeft.setPosition(pos_x, pos_y);
	topRight.setPosition(pos_x + width - 24, pos_y);
	bottomLeft.setPosition(pos_x, pos_y + height - 24);
	bottomRight.setPosition(pos_x + width - 24, pos_y + height - 24);

	//sides
	topBorder.setTexture(buttonTexture);
	topBorder.setTextureRect(sf::IntRect(16, 0, 16, 24));
	bottomBorder.setTexture(buttonTexture);
	bottomBorder.setTextureRect(sf::IntRect(16, 24, 16, 24));
	leftBorder.setTexture(buttonTexture);
	leftBorder.setTextureRect(sf::IntRect(0, 16, 24, 16));
	rightBorder.setTexture(buttonTexture);
	rightBorder.setTextureRect(sf::IntRect(24, 16, 24, 16));

	float verticalScale = (height - 48) / 16.0f;
	float horizontalScale = (width - 48) / 16.0f;

	topBorder.setPosition(pos_x + 24, pos_y);
	topBorder.setScale(horizontalScale, 1);
	bottomBorder.setPosition(pos_x + 24, pos_y + height - 24);
	bottomBorder.setScale(horizontalScale, 1);
	leftBorder.setPosition(pos_x, pos_y + 24);
	leftBorder.setScale(1, verticalScale);
	rightBorder.setPosition(pos_x + width - 24, pos_y + 24);
	rightBorder.setScale(1, verticalScale);
}

void Button::display(sf::RenderWindow& window)
{
	window.draw(topLeft);
	window.draw(topRight);
	window.draw(bottomLeft);
	window.draw(bottomRight);
	window.draw(topBorder);
	window.draw(bottomBorder);
	window.draw(leftBorder);
	window.draw(rightBorder);
}

bool Button::isHovered(const sf::Vector2i& mousePos) const
{
	return (mousePos.x >= position.x && mousePos.x <= position.x + width && mousePos.y >= position.y && mousePos.y <= position.y + height);
}

void Button::updateAppearance(bool hovered, int R, int G, int B)
{
	float elapsed = hoverClock.getElapsedTime().asSeconds();
	float alpha = std::min(elapsed / hoverDuration, 1.0f);
	sf::Color newColor = hovered ? sf::Color(R, G, B, static_cast<sf::Uint8>(255 * alpha)) : sf::Color::White;
	topLeft.setColor(newColor);
	topRight.setColor(newColor);
	bottomLeft.setColor(newColor);
	bottomRight.setColor(newColor);
	topBorder.setColor(newColor);
	bottomBorder.setColor(newColor);
	leftBorder.setColor(newColor);
	rightBorder.setColor(newColor);
}