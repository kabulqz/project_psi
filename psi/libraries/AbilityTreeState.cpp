#include "AbilityTreeState.hpp"
#include "AbilityTreeState.hpp"

#include "AbilityTree.hpp"
#include "Game.hpp"

const std::string PATH_TO_BORDERS_FOLDER = "src/img/borders/";
const std::string PATH_TO_ABILITIES_FOLDER = "src/img/";
const std::string border = "panel-border-013.png";
const std::string abilities = "abilities.png";
const std::string borderFile = PATH_TO_BORDERS_FOLDER + border;
const std::string abilitiesFile = PATH_TO_ABILITIES_FOLDER + abilities;

AbilityTreeState::AbilityTreeState(Game* game) : game(game)
{
	if (!abilityTreeTexture.loadFromFile("src/img/yggdrasil.png"))
	{
		std::cerr << "Cannot load yggdrasil texture\n";
		return;
	}

	game->changeViewZoom(1.f);

	int verticalMargin = 10;
	abilityTreeSprite.setTexture(abilityTreeTexture);
	float scaleFactor = (720.0f - 2 * verticalMargin)/ abilityTreeTexture.getSize().y;
	abilityTreeSprite.setScale(scaleFactor, scaleFactor);

	float posX = (1280 - (abilityTreeTexture.getSize().x * scaleFactor)) / 2.0f;
	float posY = 0.0f;
	abilityTreeSprite.setPosition(posX, posY + verticalMargin);

	// Load the saved ability tree from the game's save
	abilityTree = game->getSave().getAbilityTree();

	sf::Image cursorDefault;
	sf::Image cursorBuyable;
	sf::Image cursorLocked;

	if (!cursorDefault.loadFromFile("src/img/cursor_default.png") || !cursorBuyable.loadFromFile("src/img/cursor_buyable_ability.png") || !cursorLocked.loadFromFile("src/img/cursor_locked_ability.png"))
	{
		std::cerr << "Cannot load cursors png files\n";
	}

	defaultCursor.loadFromPixels(cursorDefault.getPixelsPtr(), cursorDefault.getSize(), { 0, 0 });
	buyableCursor.loadFromPixels(cursorBuyable.getPixelsPtr(), cursorBuyable.getSize(), { 16, 16 });
	lockedCursor.loadFromPixels(cursorLocked.getPixelsPtr(), cursorLocked.getSize(), { 16, 16 });

	if (!vhsShader.loadFromFile("libraries/vhs_effect.frag", sf::Shader::Fragment)) return;
	shaderClock.restart();
}

//handler for specific windows to appear in the main frame 
void AbilityTreeState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	mousePos = sf::Mouse::getPosition(window);
	

	while (eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		{
			Save save = game->getSave();
			save.write();
			game->setSave(save);
			window.setMouseCursor(defaultCursor);
			game->changeState(std::make_unique<TransitionState>(game, GAME_BOARD, std::make_unique<AbilityTreeState>(game)));
		}
	}
}

//updater for elements corresponding to specific screen
void AbilityTreeState::update()
{
	float elapsedTime = shaderClock.getElapsedTime().asSeconds();
	vhsShader.setUniform("time", elapsedTime);
	vhsShader.setUniform("resolution", sf::Vector2f(1280, 720));
}

void AbilityTreeState::renderAbilities(
	sf::RenderTarget& renderTarget,
	sf::RenderWindow& window,
	const std::shared_ptr<Ability>& ability,
	bool& isHoveredOverAnyAbility,
	CursorState& currentCursorState,
	const sf::Shader* shader)
{
	ability->display(renderTarget, shader);

	if (ability->isHovered(mousePos))
	{
		isHoveredOverAnyAbility = true;

		CursorState newCursorState = {};
		switch (ability->getUnlockStatus())
		{
		case Ability::unlockStatus::UNLOCKED:
			newCursorState = CursorState::Default;
			break;
		case Ability::unlockStatus::BUYABLE:
			newCursorState = CursorState::Buyable;
			break;
		case Ability::unlockStatus::LOCKED:
			newCursorState = CursorState::Locked;
			break;
		}

		if (newCursorState != currentCursorState)
		{
			currentCursorState = newCursorState;
			switch (currentCursorState)
			{
			case CursorState::Default:
				window.setMouseCursor(defaultCursor);
				break;
			case CursorState::Buyable:
				window.setMouseCursor(buyableCursor);
				break;
			case CursorState::Locked:
				window.setMouseCursor(lockedCursor);
				break;
			}
		}
	}

	for (auto& child : ability->getChildren())
	{
		renderAbilities(renderTarget, window, child, isHoveredOverAnyAbility, currentCursorState, abilityTree->getShader());
	}
}


//function rendering screen
void AbilityTreeState::render(sf::RenderWindow& window)
{
	sf::RenderTexture renderTexture;
	if (!renderTexture.create(window.getSize().x, window.getSize().y))
	{
		std::cerr << "Cannot create render texture\n";
		return;
	}

	// Clear and draw the base sprite
	renderTexture.clear();
	renderTexture.draw(abilityTreeSprite);

	bool isHoveredOverAnyAbility = false;
	CursorState currentCursorState = CursorState::Default;

	// Render abilities into the render texture
	if (abilityTree->getRoot())
	{
		renderAbilities(renderTexture, window, abilityTree->getRoot(), isHoveredOverAnyAbility, currentCursorState, abilityTree->getShader());
	}

	// Update the cursor if no ability is hovered
	if (!isHoveredOverAnyAbility)
	{
		window.setMouseCursor(defaultCursor);
	}

	// Display the render texture onto the window
	renderTexture.display();
	sf::Sprite screenSprite(renderTexture.getTexture());

	// Apply shader and draw to the main window
	window.clear();
	window.draw(screenSprite, &vhsShader);
	window.display();
}

void AbilityTreeState::renderToTexture(sf::RenderTexture& texture)
{
	// Clear the render texture
	texture.clear();

	// Draw the background (ability tree sprite)
	texture.draw(abilityTreeSprite);

	// Handle rendering of abilities
	CursorState currentCursorState = CursorState::Default;

	// Use a dummy RenderWindow for compatibility
	sf::RenderWindow dummyWindow(sf::VideoMode(1, 1), "Dummy Window", sf::Style::None);

	if (abilityTree->getRoot())
	{
		bool isHoveredOverAnyAbility = false;
		renderAbilities(texture, dummyWindow, abilityTree->getRoot(), isHoveredOverAnyAbility, currentCursorState, abilityTree->getShader());
	}

	// Finalize the render texture
	texture.display();
}