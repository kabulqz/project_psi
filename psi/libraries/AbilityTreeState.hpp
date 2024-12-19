#pragma once

#include "State.hpp"
#include "AbilityTree.hpp"
#include "TileMap.hpp"

class AbilityTreeState : public State
{
private:
	Game* game;
	Save* save;
	sf::Vector2i mousePos;
	sf::Cursor defaultCursor;
	sf::Cursor buyableCursor;
	sf::Cursor lockedCursor;

	sf::Texture abilityTreeTexture;
	sf::Sprite abilityTreeSprite;

	Button backButton;

	sf::Shader vhsShader;
	sf::Clock shaderClock;

	std::shared_ptr<AbilityTree> abilityTree;
	enum class CursorState
	{
		Default,
		Buyable,
		Locked
	};
	void renderAbilities(sf::RenderTarget& renderTarget, sf::RenderWindow& window, const std::shared_ptr<Ability>& ability,bool& isHoveredOverAnyAbility, CursorState& currentCursorState, const sf::Shader* shader);
public:
	//handler for specific windows to appear in the main frame 
	void handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& databasez) override;
	//updater for elements corresponding to specific screen
	void update() override;
	//function rendering screen
	void render(sf::RenderWindow& window) override;
	void renderToTexture(sf::RenderTexture& texture) override;

	AbilityTreeState(Game* game);
};