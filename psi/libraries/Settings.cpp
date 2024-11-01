#include "Settings.hpp"

#include <functional>

//default values
//values from 0 - 1
float Settings::general_audio = 1;
//values from 0 - 100
float Settings::ui_audio = 20;


void Settings::initialize()
{//this function is meant to load database, general volume level etc.
	//img database for skills and cards generally
	//https://chatgpt.com/share/6718f803-c458-8002-b648-1b4b630eaf9a

	//initializing volume settings from save - if not on save then standard
}

Button::Button(int buttonWidth, int buttonHeight, const std::string& path_to_file, const int& pos_x, const int& pos_y): width(buttonWidth), height(buttonHeight)
{
	if(!buttonTexture.loadFromFile(path_to_file))
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
