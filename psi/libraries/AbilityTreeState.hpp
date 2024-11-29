#pragma once

#include "State.hpp"

class Ability : public Button
{
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	sf::Shader shader;
	bool unlocked = true;
	bool buyable = false;
public:
	Ability(const int& pos_x, const int& pos_y, int buttonWidth, int buttonHeight, const std::string& borderPath, const std::string& backgroundPath, const sf::Vector2i& gridPosition, const int& cellSize);

	void setUnlocked(bool isUnlocked)
	{
		unlocked = isUnlocked;
	}
	bool isUnlocked() const { return unlocked; }
	void setBuyable(bool isBuyable) { buyable = isBuyable; }
	bool isBuyable() const { return buyable; }
	void display(sf::RenderWindow& window);
};

class AbilityTreeState : public State
{
private:
	Game* game;
	Ability skillButtonTest;
public:
	//handler for specific windows to appear in the main frame 
	void handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& databasez) override;
	//updater for elements corresponding to specific screen
	void update() override;
	//function rendering screen
	void render(sf::RenderWindow& window) override;

	AbilityTreeState(Game* game);
};