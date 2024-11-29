#include "AbilityTreeState.hpp"
#include "Game.hpp"

Ability::Ability(const int& pos_x, const int& pos_y, int buttonWidth, int buttonHeight, const std::string& borderPath,
				 const std::string& backgroundPath, const sf::Vector2i& gridPosition, const int& cellSize) : Button(pos_x, pos_y, buttonWidth, buttonHeight, borderPath)
{
	//setting the background image
	sf::IntRect textureRect{
		gridPosition.x * cellSize,	// Left
		gridPosition.y * cellSize,	// Top
		cellSize,					// Width
		cellSize					// Height
	};

	if (!backgroundTexture.loadFromFile(backgroundPath))
	{
		std::cerr << "Cannot load background from " << backgroundPath << "\n";
		return;
	}
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setTextureRect(textureRect); // Crop the texture
	backgroundSprite.setPosition(static_cast<float>(pos_x), static_cast<float>(pos_y));
	backgroundSprite.setScale(static_cast<float>(getWidth()) / cellSize, static_cast<float>(getHeight()) / cellSize);

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
	if (unlocked)
	{
		window.draw(backgroundSprite);
	}
	else
	{
		window.draw(backgroundSprite, &shader);
	}

	Button::display(window);
}

const std::string PATH_TO_BORDERS_FOLDER = "src/img/borders/";
const std::string PATH_TO_ABILITIES_FOLDER = "src/img/abilities/";

AbilityTreeState::AbilityTreeState(Game* game) : game(game),
skillButtonTest(100, 100, 100, 100, PATH_TO_BORDERS_FOLDER + "panel-border-027.png",PATH_TO_ABILITIES_FOLDER + "Tileset 48.jpg", sf::Vector2i(0, 0), 48)
{
	
}

//handler for specific windows to appear in the main frame 
void AbilityTreeState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
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
	skillButtonTest.display(window);

	window.display();
}