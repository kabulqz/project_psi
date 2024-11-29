#include "AbilityTreeState.hpp"
#include "AbilityTreeState.hpp"
#include "Game.hpp"

Ability::Ability(const int& pos_x, const int& pos_y, int buttonWidth, int buttonHeight, const std::string& borderPath,
				 const std::string& backgroundPath, const sf::Vector2i& gridPosition) : Button(pos_x, pos_y, buttonWidth, buttonHeight, borderPath)
{
	//setting the background image
	sf::IntRect textureRect{
		gridPosition.x * 48,	// Left
		gridPosition.y * 48,	// Top
		48,						// Width
		48						// Height
	};

	if (!backgroundTexture.loadFromFile(backgroundPath))
	{
		std::cerr << "Cannot load background from " << backgroundPath << "\n";
		return;
	}
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setTextureRect(textureRect); // Crop the texture
	backgroundSprite.setPosition(static_cast<float>(pos_x), static_cast<float>(pos_y));
	backgroundSprite.setScale(static_cast<float>(getWidth()) / 48, static_cast<float>(getHeight()) / 48);

	if (!shader.loadFromFile("libraries/shader.frag", sf::Shader::Fragment))
	{
		std::cerr << "Cannot load shader from libraries/grayscale.frag\n";
	}
	else
	{
		std::cout << "Shader loaded successfully\n";
	}
}

void Ability::display(sf::RenderWindow& window)
{
	switch (status)
	{
	case unlockStatus::UNLOCKED: // 0
		Button::setColor("2FBF71");
		window.draw(backgroundSprite);
		break;
	case unlockStatus::BUYABLE: // 1
		Button::setColor("F3CA40");
		window.draw(backgroundSprite, &shader);
		break;
	case unlockStatus::LOCKED: // 2
		Button::setColor("D90429");
		window.draw(backgroundSprite, &shader);
		break;
	}

	Button::display(window);
}

const std::string PATH_TO_BORDERS_FOLDER = "src/img/borders/";
const std::string PATH_TO_ABILITIES_FOLDER = "src/img/";

AbilityTreeState::AbilityTreeState(Game* game) : game(game),
startAbility(616, 605, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(12, 2)),
heal1(617, 510, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(9, 8)),
heal2(621, 403, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(10, 8)),
heal3(620, 300, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(11, 8)),
heal4(624, 188, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(12, 8)),
fireball1(498, 392, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(6, 0)),
fireball2(703, 244, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(5, 0)),
freeze1(531, 317, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(11, 6)),
freeze2(805, 250, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(12, 6)),
plusSpeed1(694, 438, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(14, 8)),
plusSpeed2(434, 345, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(0, 9)),
buffMeleeUnits1(553, 443, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(4, 9)),
buffMeleeUnits2(815, 347, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(6, 9)),
buffMeleeUnits3(656, 101, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(5, 9)),
buffRangedUnits(774, 192, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(8, 9)),
maxCardHand(432, 242, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(9, 9)),
maxHealth1(706, 331, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(10, 9)),
maxHealth2(538, 218, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(11, 9)),
maxEnergy1(755, 392, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(3, 7)),
maxEnergy2(584, 101, 48, 48, PATH_TO_BORDERS_FOLDER + "panel-border-013.png", PATH_TO_ABILITIES_FOLDER + "abilities.png", sf::Vector2i(2, 7))
{
	if (!abilityTreeTexture.loadFromFile("src/img/yggdrasil.png"))
	{
		std::cerr << "Cannot load yggdrasil texture\n";
		return;
	}

	int verticalMargin = 10;
	abilityTreeSprite.setTexture(abilityTreeTexture);
	float scaleFactor = (720.0f - 2 * verticalMargin)/ abilityTreeTexture.getSize().y;
	abilityTreeSprite.setScale(scaleFactor, scaleFactor);

	float posX = (1280 - (abilityTreeTexture.getSize().x * scaleFactor)) / 2.0f;
	float posY = 0.0f;
	abilityTreeSprite.setPosition(posX, posY + verticalMargin);
	abilities[0]->setUnlockStatus(Ability::unlockStatus::UNLOCKED);
	abilities[1]->setUnlockStatus(Ability::unlockStatus::BUYABLE);
	// Saving the states of the skills would be in numbers: 0 - unlocked, 1 - buyable, 2 - locked
	// For now no need

	if (!cursorDefault.loadFromFile("src/img/cursor_default.png") || !cursorBuyable.loadFromFile("src/img/cursor_buyable_ability.png") || !cursorLocked.loadFromFile("src/img/cursor_locked_ability.png"))
	{
		std::cerr << "Cannot load cursors png files\n";
	}
}

//handler for specific windows to appear in the main frame 
void AbilityTreeState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	mousePos = sf::Mouse::getPosition(window);
	bool cursorChanged = false;

	for (auto ability : abilities)
	{
		ability->handleHoverState(mousePos);
		if (ability->isHovered(mousePos))
		{
			if (ability->getUnlockStatus() == 1) // Buyable
			{
				if (cursor.loadFromPixels(cursorBuyable.getPixelsPtr(), cursorBuyable.getSize(), sf::Vector2u(16, 16)))
				{
					window.setMouseCursor(cursor);
					cursorChanged = true;
				}
			}
			else if (ability->getUnlockStatus() == 2) // Locked
			{
				if (cursor.loadFromPixels(cursorLocked.getPixelsPtr(), cursorLocked.getSize(), sf::Vector2u(16, 16)))
				{
					window.setMouseCursor(cursor);
					cursorChanged = true;
				}
			}
			else
			{
				if (cursor.loadFromPixels(cursorDefault.getPixelsPtr(), cursorDefault.getSize(), sf::Vector2u(0, 0)))
				{
					window.setMouseCursor(cursor);
					cursorChanged = true;
				}
			}
			break;
		}
	}

	if (!cursorChanged)
	{
		if (cursor.loadFromPixels(cursorDefault.getPixelsPtr(), cursorDefault.getSize(), sf::Vector2u(0, 0)))
		{
			window.setMouseCursor(cursor);
		}
	}

	while (eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
	}
}

//updater for elements corresponding to specific screen
void AbilityTreeState::update()
{
	
}

//function rendering screen
void AbilityTreeState::render(sf::RenderWindow& window)
{
	window.clear();
	//draw skills here
	window.draw(abilityTreeSprite);
	//draw abilities
	for (auto ability : abilities)
	{
		ability->display(window);
	}

	window.display();
}