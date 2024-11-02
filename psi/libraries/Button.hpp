#pragma once

#include "Settings.hpp"

class Button
{
private:
	sf::Texture buttonTexture;
	//size values
	int width;
	int height;
	//sprites corners
	sf::Sprite topLeft;
	sf::Sprite topRight;
	sf::Sprite bottomLeft;
	sf::Sprite bottomRight;
	//sprites borders
	sf::Sprite topBorder;
	sf::Sprite bottomBorder;
	sf::Sprite leftBorder;
	sf::Sprite rightBorder;
	//setting up the sprites 
	void setUpCornerSprites(int pos_x, int pos_y);
	void setUpBorderSprites(int pos_x, int pos_y);
	//button position
	struct Position
	{
		int x;
		int y;
	};
	Position position;
	bool hovered;
	//duration to change color
	float hoverDuration = 0.22f;
	void applyColorToSprites(const sf::Color& color);
public:
	sf::Clock hoverClock;
	Button(const int& pos_x, const int& pos_y, int buttonWidth, int buttonHeight, const std::string& path_to_file);
	void display(sf::RenderWindow& window) const;
	bool isHovered(const sf::Vector2i& mousePos) const;
	void updateAppearance(bool isHovered, const std::string& hexColor);
	void updateAppearanceWithBaseColor(bool isHovered, const std::string& baseHexColor, const std::string& targetHexColor);
	void setHovered(bool isHovered);
	bool getHovered() const { return hovered; }
};