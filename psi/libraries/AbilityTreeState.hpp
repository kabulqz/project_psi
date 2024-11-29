#pragma once

#include "State.hpp"

class Ability : public Button
{
	enum class unlockStatus;
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	sf::Shader shader;
	unlockStatus status = unlockStatus::LOCKED;
public:
	enum class unlockStatus
	{
		UNLOCKED,	// 0
		BUYABLE,	// 1
		LOCKED		// 2
	};

	Ability(const int& pos_x, const int& pos_y, int buttonWidth, int buttonHeight, const std::string& borderPath, const std::string& backgroundPath, const sf::Vector2i& gridPosition);

	int getUnlockStatus() { return static_cast<int>(status); }
	void setUnlockStatus(unlockStatus status) { this->status = status; }
	void setUnlockStatus(int status) { this->status = static_cast<unlockStatus>(status); }
	void display(sf::RenderWindow& window);
};

class AbilityTreeState : public State
{
private:
	Game* game;
	sf::Vector2i mousePos;
	sf::Cursor cursor;

	sf::Image cursorDefault;
	sf::Image cursorBuyable;
	sf::Image cursorLocked;

	sf::Texture abilityTreeTexture;
	sf::Sprite abilityTreeSprite;

	Ability startAbility;	// 12, 2
	Ability heal1;			// 9, 8
	Ability heal2;			// 10, 8
	Ability heal3;			// 11, 8
	Ability heal4;			// 12, 8
	Ability fireball1;		// 6, 0
	Ability fireball2;		// 5, 0
	Ability freeze1;		// 11, 6
	Ability freeze2;		// 12, 6
	Ability plusSpeed1;		// 14, 8
	Ability plusSpeed2;		// 0, 9

	Ability buffMeleeUnits1;// 4, 9
	Ability buffMeleeUnits2;// 6, 9
	Ability buffMeleeUnits3;// 5, 9
	Ability buffRangedUnits;// 8, 9
	Ability maxCardHand;	// 9, 9
	Ability maxHealth1;		// 10, 9
	Ability maxHealth2;		// 11, 9
	Ability maxEnergy1;		// 3, 7
	Ability maxEnergy2;		// 2, 7

	std::vector<Ability*> abilities = {
		&startAbility,
		&heal1,
		&heal2,
		&heal3,
		&heal4,
		&fireball1,
		&fireball2,
		&freeze1,
		&freeze2,
		&plusSpeed1,
		&plusSpeed2,
		&buffMeleeUnits1,
		&buffMeleeUnits2,
		&buffMeleeUnits3,
		&buffRangedUnits,
		&maxCardHand,
		&maxHealth1,
		&maxHealth2,
		&maxEnergy1,
		&maxEnergy2
	};

public:
	//handler for specific windows to appear in the main frame 
	void handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& databasez) override;
	//updater for elements corresponding to specific screen
	void update() override;
	//function rendering screen
	void render(sf::RenderWindow& window) override;

	AbilityTreeState(Game* game);
};