#pragma once

#include "Settings.hpp"

class State
{
public:
	//handler for specific windows to appear in the main frame 
	virtual void handleInput(sf::RenderWindow& window) = 0;
	//updater for elements corresponding to specific screen
	virtual void update() = 0;
	//function rendering screen
	virtual void render(sf::RenderWindow& window) = 0;
	virtual ~State() = default;
};

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
	struct Position
	{
		int x;
		int y;
	};
	Position position;
	//duration to change color
	float hoverDuration = 0.22f;
public:
	bool hovered;
	sf::Clock hoverClock;
	Button(int buttonWidth, int buttonHeight, const std::string& path_to_file, const int& pos_x, const int& pos_y);
	void display(sf::RenderWindow& window);
	bool isHovered(const sf::Vector2i& mousePos) const;
	void updateAppearance(bool hovered, int R, int G, int B);
};