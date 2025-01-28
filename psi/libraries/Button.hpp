#pragma once

#include "Settings.hpp"

class Button
{
protected:
	sf::Texture buttonTexture;
	sf::RectangleShape buttonBackground;
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
	sf::Vector2f position;
	bool hovered;
	//duration to change color
	sf::Clock hoverClock;
	float hoverDuration = 0.22f;
	//font and text holders
	sf::Text buttonText;
	sf::Font buttonFont;
	std::string buttonFontFilePath;
	//indicates if the button is clickable or if it is visible
	bool enabled;
	bool visible;
	//background sprite
public:
	Button(const int& pos_x, const int& pos_y, int buttonWidth, int buttonHeight, const std::string& borderPath);
	void resizeTo(int newWidth, int newHeight);
	void setEnabled(bool isEnabled);
	bool isEnabled() const;
	void setVisible(bool isVisible);
	bool isVisible() const;
	void setText(const std::string& text, const std::string& fontPath, unsigned int characterSize);
	std::string getText() const
	{
		if (buttonText.getString() != "") return buttonText.getString();
		return "";
	}

	void setHovered(bool isHovered);
	bool isHovered(const sf::Vector2i& mousePos) const;
	bool getHovered() const { return hovered; }
	bool isClickable() const { return (enabled && visible); }
	void updateAppearance(const std::string& hexColor);
	void updateAppearanceWithBaseColor(const std::string& baseHexColor, const std::string& targetHexColor);
	void display(sf::RenderTarget& window);
	void setBackgroundColor(const sf::Color& color);
	void setBackgroundColor(const std::string& hexColor);
	void setColor(const sf::Color& color);
	void setColor(const std::string& hexColor);
	void handleHoverState(sf::Vector2i mousePosition);
	sf::FloatRect getBounds() const;
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	sf::Vector2f getPosition() const { return position; }
	virtual void setPosition(const sf::Vector2f& pos)
	{
		position = pos;
		buttonBackground.setPosition(pos);
	}
};