#include "Button.hpp"

//Constructor for initializing the Button object
Button::Button(const int& pos_x, const int& pos_y, int buttonWidth, int buttonHeight, const std::string& path_to_file) : width(buttonWidth), height(buttonHeight), position{ pos_x, pos_y }, hovered(false), enabled(true)
{
	//loading .png file
	if (!buttonTexture.loadFromFile(path_to_file))
	{
		std::cerr << "Cannot load button from "<<path_to_file<<"\n";
		return;
	}

	//setting up the sprites from .png file
	setUpCornerSprites(pos_x, pos_y);
	setUpBorderSprites(pos_x, pos_y);

	//setting default properties for the button text
	buttonText.setFillColor(sf::Color::White);//default text color
}

//Method to set up corner sprites using the button texture
void Button::setUpCornerSprites(int pos_x, int pos_y)
{
	//Top left corner sprite
	topLeft.setTexture(buttonTexture);
	topLeft.setTextureRect(sf::IntRect(0, 0, 24, 24));
	topLeft.setPosition(static_cast<float>(pos_x), static_cast<float>(pos_y));

	//Top right corner sprite
	topRight.setTexture(buttonTexture);
	topRight.setTextureRect(sf::IntRect(24, 0, 24, 24));
	topRight.setPosition(static_cast<float>(pos_x + width - 24), static_cast<float>(pos_y));

	//Bottom left corner sprite
	bottomLeft.setTexture(buttonTexture);
	bottomLeft.setTextureRect(sf::IntRect(0, 24, 24, 24));
	bottomLeft.setPosition(static_cast<float>(pos_x), static_cast<float>(pos_y + height - 24));

	//Bottom right corner sprite
	bottomRight.setTexture(buttonTexture);
	bottomRight.setTextureRect(sf::IntRect(24, 24, 24, 24));
	bottomRight.setPosition(static_cast<float>(pos_x + width - 24), static_cast<float>(pos_y + height - 24));
}

//Method to set up border sprites using the button texture
void Button::setUpBorderSprites(int pos_x, int pos_y)
{
	//Calculate scaling factors for the top/bottom and left/right borders
	float verticalScale = (height - 48) / 16.0f;
	float horizontalScale = (width - 48) / 16.0f;

	//Top border sprite
	topBorder.setTexture(buttonTexture);
	topBorder.setTextureRect(sf::IntRect(16, 0, 16, 24));
	topBorder.setPosition(static_cast<float>(pos_x + 24), static_cast<float>(pos_y));
	topBorder.setScale(horizontalScale, 1);

	//Bottom border sprite
	bottomBorder.setTexture(buttonTexture);
	bottomBorder.setTextureRect(sf::IntRect(16, 24, 16, 24));
	bottomBorder.setPosition(static_cast<float>(pos_x + 24), static_cast<float>(pos_y + height - 24));
	bottomBorder.setScale(horizontalScale, 1);

	//Left border sprite
	leftBorder.setTexture(buttonTexture);
	leftBorder.setTextureRect(sf::IntRect(0, 16, 24, 16));
	leftBorder.setPosition(static_cast<float>(pos_x), static_cast<float>(pos_y + 24));
	leftBorder.setScale(1, verticalScale);

	//Right border sprite
	rightBorder.setTexture(buttonTexture);
	rightBorder.setTextureRect(sf::IntRect(24, 16, 24, 16));
	rightBorder.setPosition(static_cast<float>(pos_x + width - 24), static_cast<float>(pos_y + 24));
	rightBorder.setScale(1, verticalScale);
}

//Methods to tell program if the button is enabled to be clicked or not
void Button::setEnabled(bool isEnabled)
{
	enabled = isEnabled;
}

bool Button::isEnabled() const
{
	return enabled;
}

//Method to set the text on the button
void Button::setText(const std::string& text, const std::string& fontPath, unsigned int characterSize)
{
	//Load the font from the specified file path
	if (!buttonFont.loadFromFile(fontPath))
	{
		std::cerr << "Cannot load font from " << fontPath << "\n";
		return;
	}

	//Set the font, text string, and character size for the text on the button
	buttonText.setFont(buttonFont);
	buttonText.setString(text);
	buttonText.setCharacterSize(characterSize);

	//Calculate the position to center the text within the button
	sf::FloatRect textBounds = buttonText.getLocalBounds();
	float textX = static_cast<float>(position.x) + (static_cast<float>(width) - textBounds.width) / 2 - textBounds.left;
	float textY = static_cast<float>(position.y) + (static_cast<float>(height) - textBounds.height) / 2 - textBounds.top;
	buttonText.setPosition(textX, textY);
}

//Method to set the hovered state and restart the hover clock if it changes
void Button::setHovered(bool isHovered)
{
	if (hovered != isHovered)
	{
		hovered = isHovered;
		hoverClock.restart();//Restart the hover timer
	}
}

//Method to check if the button is hovered by the mouse
bool Button::isHovered(const sf::Vector2i& mousePos) const
{
	//Check if the mouse position is within the button's bounds
	return (mousePos.x >= position.x && mousePos.x <= position.x + width && mousePos.y >= position.y && mousePos.y <= position.y + height);
}

//Method to update the button's appearance when hovered or not
void Button::updateAppearance(bool isHovered, const std::string& hexColor)
{
	//Check if the button is enabled
	if (!enabled)
	{
		//Set the color to dark gray if the button is disabled
		sf::Color disabledColor(52, 58, 64);
		setColor(disabledColor);
		return;
	}

	//Check if the hovered state has changed
	if(hovered != isHovered)
	{
		setHovered(isHovered);
	}

	//Calculate the transition alpha based on hover duration
	float elapsed = hoverClock.getElapsedTime().asSeconds();
	float alpha = std::min(elapsed / hoverDuration, 1.0f);

	//Parse the hex color to RGB components
	int R = std::stoi(hexColor.substr(0, 2), nullptr, 16);
	int G = std::stoi(hexColor.substr(2, 2), nullptr, 16);
	int B = std::stoi(hexColor.substr(4, 2), nullptr, 16);

	//Determine the new color based on hover state and transition
	sf::Color newColor;
	if(this->hovered)
	{
		//Transition from white to the target color
		newColor = sf::Color(
			static_cast<sf::Uint8>(255 + (R - 255) * alpha),
			static_cast<sf::Uint8>(255 + (G - 255) * alpha),
			static_cast<sf::Uint8>(255 + (B - 255) * alpha),
			255
		);
	}
	else
	{
		//Transition from the target color back to white
		newColor = sf::Color(
			static_cast<sf::Uint8>(R + (255 - R) * alpha),
			static_cast<sf::Uint8>(G + (255 - G) * alpha),
			static_cast<sf::Uint8>(B + (255 - B) * alpha),
			255
		);
	}

	//Apply the new color to all button sprites
	setColor(newColor);
}

//Method to update the appearance of the button using a base and target color
void Button::updateAppearanceWithBaseColor(bool isHovered, const std::string& baseHexColor, const std::string& targetHexColor)
{
	//Check if the button is enabled
	if (!enabled)
	{
		//Set the color to dark gray if the button is disabled
		sf::Color disabledColor(52, 58, 64);
		setColor(disabledColor);
		return;
	}

	//Check if the hovered state has changed
	if (hovered != isHovered)
	{
		setHovered(isHovered);
	}

	//Calculate the transition alpha based on hover duration
	float elapsed = hoverClock.getElapsedTime().asSeconds();
	float alpha = std::min(elapsed / hoverDuration, 1.0f);

	//Parse the base and target hex colors to RGB components
	int baseR = std::stoi(baseHexColor.substr(0, 2), nullptr, 16);
	int baseG = std::stoi(baseHexColor.substr(2, 2), nullptr, 16);
	int baseB = std::stoi(baseHexColor.substr(4, 2), nullptr, 16);

	int targetR = std::stoi(targetHexColor.substr(0, 2), nullptr, 16);
	int targetG = std::stoi(targetHexColor.substr(2, 2), nullptr, 16);
	int targetB = std::stoi(targetHexColor.substr(4, 2), nullptr, 16);

	//Determine the new color based on hover state and transition
	sf::Color newColor;
	if (this->hovered)
	{
		//Transition from the base color to the target color when hovered
		newColor = sf::Color(
			static_cast<sf::Uint8>(baseR + (targetR - baseR) * alpha),
			static_cast<sf::Uint8>(baseG + (targetG - baseG) * alpha),
			static_cast<sf::Uint8>(baseB + (targetB - baseB) * alpha),
			255
		);
	}
	else
	{
		//Transition from the target color back to the base color
		newColor = sf::Color(
			static_cast<sf::Uint8>(targetR + (baseR - targetR) * alpha),
			static_cast<sf::Uint8>(targetG + (baseG - targetG) * alpha),
			static_cast<sf::Uint8>(targetB + (baseB - targetB) * alpha),
			255
		);
	}

	//Apply the new color to all button sprites
	setColor(newColor);
}

//Method to draw the button on the window
void Button::display(sf::RenderWindow& window) const
{
	//Draw corner sprites
	window.draw(topLeft);
	window.draw(topRight);
	window.draw(bottomLeft);
	window.draw(bottomRight);

	//Draw border sprites
	window.draw(topBorder);
	window.draw(bottomBorder);
	window.draw(leftBorder);
	window.draw(rightBorder);

	//Draw the button text
	window.draw(buttonText);
}

//Method to apply a given color to all button sprites
void Button::setColor(const sf::Color& color)
{
	topLeft.setColor(color);
	topRight.setColor(color);
	bottomLeft.setColor(color);
	bottomRight.setColor(color);
	topBorder.setColor(color);
	bottomBorder.setColor(color);
	leftBorder.setColor(color);
	rightBorder.setColor(color);

	buttonText.setFillColor(color);
}

void Button::setColor(const std::string& hexColor)
{
	//Parse the hex color to RGB components
	int R = std::stoi(hexColor.substr(0, 2), nullptr, 16);
	int G = std::stoi(hexColor.substr(2, 2), nullptr, 16);
	int B = std::stoi(hexColor.substr(4, 2), nullptr, 16);

	sf::Color color(static_cast<sf::Uint8>(R), static_cast<sf::Uint8>(G), static_cast<sf::Uint8>(B));

	topLeft.setColor(color);
	topRight.setColor(color);
	bottomLeft.setColor(color);
	bottomRight.setColor(color);
	topBorder.setColor(color);
	bottomBorder.setColor(color);
	leftBorder.setColor(color);
	rightBorder.setColor(color);

	buttonText.setFillColor(color);
}