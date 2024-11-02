#include "Button.hpp"

Button::Button(const int& pos_x, const int& pos_y, int buttonWidth, int buttonHeight, const std::string& path_to_file) : width(buttonWidth), height(buttonHeight), position{ pos_x, pos_y }, hovered(false)
{
	if (!buttonTexture.loadFromFile(path_to_file))
	{
		std::cerr << "Cannot load button from "<<path_to_file<<"\n";
		return;
	}

	setUpCornerSprites(pos_x, pos_y);
	setUpBorderSprites(pos_x, pos_y);
}

void Button::setUpCornerSprites(int pos_x, int pos_y)
{
	//Top left corner
	topLeft.setTexture(buttonTexture);
	topLeft.setTextureRect(sf::IntRect(0, 0, 24, 24));
	topLeft.setPosition(static_cast<float>(pos_x), static_cast<float>(pos_y));

	//Top right corner
	topRight.setTexture(buttonTexture);
	topRight.setTextureRect(sf::IntRect(24, 0, 24, 24));
	topRight.setPosition(static_cast<float>(pos_x + width - 24), static_cast<float>(pos_y));

	//Bottom left corner
	bottomLeft.setTexture(buttonTexture);
	bottomLeft.setTextureRect(sf::IntRect(0, 24, 24, 24));
	bottomLeft.setPosition(static_cast<float>(pos_x), static_cast<float>(pos_y + height - 24));

	//Bottom right corner
	bottomRight.setTexture(buttonTexture);
	bottomRight.setTextureRect(sf::IntRect(24, 24, 24, 24));
	bottomRight.setPosition(static_cast<float>(pos_x + width - 24), static_cast<float>(pos_y + height - 24));
}

void Button::setUpBorderSprites(int pos_x, int pos_y)
{
	//calculate scaling factors for borders
	float verticalScale = (height - 48) / 16.0f;
	float horizontalScale = (width - 48) / 16.0f;

	//top border
	topBorder.setTexture(buttonTexture);
	topBorder.setTextureRect(sf::IntRect(16, 0, 16, 24));
	topBorder.setPosition(static_cast<float>(pos_x + 24), static_cast<float>(pos_y));
	topBorder.setScale(horizontalScale, 1);

	//bottom border
	bottomBorder.setTexture(buttonTexture);
	bottomBorder.setTextureRect(sf::IntRect(16, 24, 16, 24));
	bottomBorder.setPosition(static_cast<float>(pos_x + 24), static_cast<float>(pos_y + height - 24));
	bottomBorder.setScale(horizontalScale, 1);

	//left border
	leftBorder.setTexture(buttonTexture);
	leftBorder.setTextureRect(sf::IntRect(0, 16, 24, 16));
	leftBorder.setPosition(static_cast<float>(pos_x), static_cast<float>(pos_y + 24));
	leftBorder.setScale(1, verticalScale);

	//right border
	rightBorder.setTexture(buttonTexture);
	rightBorder.setTextureRect(sf::IntRect(24, 16, 24, 16));
	rightBorder.setPosition(static_cast<float>(pos_x + width - 24), static_cast<float>(pos_y + 24));
	rightBorder.setScale(1, verticalScale);
}

void Button::display(sf::RenderWindow& window) const
{
	//draw corners
	window.draw(topLeft);
	window.draw(topRight);
	window.draw(bottomLeft);
	window.draw(bottomRight);

	//draw borders
	window.draw(topBorder);
	window.draw(bottomBorder);
	window.draw(leftBorder);
	window.draw(rightBorder);
}

bool Button::isHovered(const sf::Vector2i& mousePos) const
{
	return (mousePos.x >= position.x && mousePos.x <= position.x + width && mousePos.y >= position.y && mousePos.y <= position.y + height);
}

void Button::setHovered(bool isHovered)
{
	if (hovered != isHovered)
	{
		hovered = isHovered;
		hoverClock.restart();
	}
}

void Button::updateAppearance(bool isHovered, int R, int G, int B)
{
	if(hovered != isHovered)
	{
		setHovered(isHovered);
	}

	float elapsed = hoverClock.getElapsedTime().asSeconds();
	float alpha = std::min(elapsed / hoverDuration, 1.0f);

	//determine color to use
	sf::Color newColor;
	if(this->hovered)
	{
		newColor = sf::Color(
			static_cast<sf::Uint8>(255 + (R - 255) * alpha),
			static_cast<sf::Uint8>(255 + (G - 255) * alpha),
			static_cast<sf::Uint8>(255 + (B - 255) * alpha),
			255
		);
	}
	else
	{
		newColor = sf::Color(
			static_cast<sf::Uint8>(R + (255 - R) * alpha),
			static_cast<sf::Uint8>(G + (255 - G) * alpha),
			static_cast<sf::Uint8>(B + (255 - B) * alpha),
			255
		);
	}

	//apply color to all sprites
	applyColorToSprites(newColor);
}

void Button::applyColorToSprites(const sf::Color& color)
{
	topLeft.setColor(color);
	topRight.setColor(color);
	bottomLeft.setColor(color);
	bottomRight.setColor(color);
	topBorder.setColor(color);
	bottomBorder.setColor(color);
	leftBorder.setColor(color);
	rightBorder.setColor(color);
}
