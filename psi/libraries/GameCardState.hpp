#pragma once

#include "State.hpp"
#include "Game.hpp"
#include "CardButton.hpp"

class GameCardState : public State
{
private:
	void adjustEffectDescriptionSize(Button& effectDescription) const;
	void adjustEffectDescriptionsLayout() const;
	void updateCardEffects(const CardButton* cardButton) const;
	bool showingEffects = false;

	Game* game;
	Save* save;

	sf::Cursor defaultCursor;
	sf::Cursor openHandCursor;
	sf::Cursor closedHandCursor;
	sf::Cursor detailsCursor;

	enum class CursorState
	{
		Default,
		OpenHand,
		ClosedHand,
		Details
	};
	void handleCursorChange(sf::RenderWindow& window, CardButton& cardButton, bool& isHoveredOverAnyCard, CursorState& cursorState) const;

	Hero* player;
	Hero* enemy;

	struct BackgroundData
	{
		sf::Glsl::Vec4 colour1; // Colours for the background shader
		sf::Glsl::Vec4 colour2; // 1 - darker, 2 - lighter, 3 - lightest
		sf::Glsl::Vec4 colour3;

		BackgroundData(sf::Glsl::Vec4 colour1, sf::Glsl::Vec4 colour2, sf::Glsl::Vec4 colour3)
			: colour1(colour1), colour2(colour2), colour3(colour3) {
		}
		BackgroundData() = default;
	};
	std::vector<BackgroundData> backgrounds = {
		{sf::Glsl::Vec4(0.30f,0.50f,0.37f,1.0f), sf::Glsl::Vec4(0.11f,0.22f,0.15f,1.0f), sf::Glsl::Vec4(0.f,0.03f,0.01f,1.0f)},
		{sf::Glsl::Vec4(0.16f,0.11f,0.34f,1.0), sf::Glsl::Vec4(0.49f,0.14f,0.65f,1.0f), sf::Glsl::Vec4(0.86f,0.23f,0.94f,1.0f)},
		{sf::Glsl::Vec4(0.67f,0.31f,0.16f,1.0f), sf::Glsl::Vec4(0.94f,0.39f,0.18f,1.0f), sf::Glsl::Vec4(0.94f,0.63f,0.32f,1.0f)},
		{sf::Glsl::Vec4(0.67f,0.41f,0.27f,1.0f), sf::Glsl::Vec4(0.44f,0.52f,0.51f,1.0f), sf::Glsl::Vec4(0.78f,0.8f,0.63f,1.0f)},
		{sf::Glsl::Vec4(0.17f,0.29f,0.45f,1.0f), sf::Glsl::Vec4(0.23f,0.49f,0.65f,1.0f), sf::Glsl::Vec4(0.54f,0.79f,0.85f,1.0f)},
		{sf::Glsl::Vec4(0.94f,0.42f,0.52f,1.0f), sf::Glsl::Vec4(0.65f,0.33f,0.48f,1.0f), sf::Glsl::Vec4(0.14f,0.12f,0.14f,1.0f)},
		{sf::Glsl::Vec4(0.74f,0.22f,0.43f,1.0f), sf::Glsl::Vec4(0.56f,0.16f,0.94f,1.0f), sf::Glsl::Vec4(0.f,0.f,0.f,1.0f)},
		{sf::Glsl::Vec4(0.02f,0.02f,0.03f,1.0f), sf::Glsl::Vec4(0.08f,0.08f,0.14f,1.0f), sf::Glsl::Vec4(0.57f,0.46f,0.67f,1.0f)},
		{sf::Glsl::Vec4(0.94f,0.94f,0.94f,1.0f), sf::Glsl::Vec4(0.65f,0.09f,0.13f,1.0f), sf::Glsl::Vec4(0.05f,0.04f,0.04f,1.0f)},
		{sf::Glsl::Vec4(0.94f,0.94f,0.94f,1.0f), sf::Glsl::Vec4(0.45f,0.45f,0.45f,1.0f), sf::Glsl::Vec4(0.05f,0.05f,0.05f,1.0f)},
		{sf::Glsl::Vec4(0.34f,0.25f,0.20f,1.0f), sf::Glsl::Vec4(0.54f,0.39f,0.33f,1.0f), sf::Glsl::Vec4(0.94f,0.69f,0.06f,1.0f)},
		{sf::Glsl::Vec4(0.04f,0.29f,0.34f,1.0f), sf::Glsl::Vec4(0.04f,0.54f,0.54f,1.0f), sf::Glsl::Vec4(0.02f,0.85f,0.76f,1.0f)},
		{sf::Glsl::Vec4(0.12f,0.54f,0.47f,1.0f), sf::Glsl::Vec4(0.18f,0.85f,0.72f,1.0f), sf::Glsl::Vec4(0.53f,0.94f,0.87f,1.0f)},
		{sf::Glsl::Vec4(0.94f,0.71f,0.01f,1.0f), sf::Glsl::Vec4(0.16f,0.16f,0.16f,1.0f), sf::Glsl::Vec4(0,0,0,1.0f)},
		{sf::Glsl::Vec4(0.18f,0.45f,0.32f,1.0f), sf::Glsl::Vec4(0.33f,0.74f,0.51f,1.0f), sf::Glsl::Vec4(0.60f,0.85f,0.7f,1.0f)},
		{sf::Glsl::Vec4(0.5f, 0.1f, 0.2f, 1.0f), sf::Glsl::Vec4(0.64f, 0.15f, 0.3f, 1.0f), sf::Glsl::Vec4(0.79f, 0.2f, 0.4f, 1.0f)},
		{sf::Glsl::Vec4(0.07f,0.07f,0.18f,1.0f), sf::Glsl::Vec4(0.23f,0.25f,0.62f,1.0f), sf::Glsl::Vec4(0.f,0.58f,0.67f,1.0f)},
		{sf::Glsl::Vec4(0.14f,0.01f,0.04f,1.0f), sf::Glsl::Vec4(0.23f,0.007f,0.07f,1.0f), sf::Glsl::Vec4(0.36f,0.01f,0.11f,1.0f)},
		{sf::Glsl::Vec4(0.f,0.007f,0.05f,1.0f), sf::Glsl::Vec4(0.003f,0.02f,0.14f,1.0f), sf::Glsl::Vec4(0.f,0.03f,0.29f,1.0f)},
		{sf::Glsl::Vec4(0.94f,0.13f,0.13f,1.0f), sf::Glsl::Vec4(0.94f,0.36f,0.15f,1.0f), sf::Glsl::Vec4(0.94f,0.70f,0.26f,1.0f)}
	};
	BackgroundData chosenBackground;

	sf::Shader backgroundShader;
	sf::Shader vhsShader;
	sf::Clock shaderClock;
	sf::Vector2i mousePos;

	std::vector<CardButton*> playerCardButtons;
	std::vector<CardButton*> enemyCardButtons;

	Button enemyHand;
	Button playerHand;
	Button enemyBattlefield;
	Button playerBattlefield;
	Button enemyDeck;
	Button playerDeck;
	Button PASS;

	Button effectDescription1;
	Button effectDescription2;
	Button effectDescription3;
	Button effectDescription4;
	Button effectDescription5;
	Button effectDescription6;
	std::vector<Button*> effectDescriptions = {
		&effectDescription1,
		&effectDescription2,
		&effectDescription3,
		&effectDescription4,
		&effectDescription5,
		&effectDescription6 };
public:
	//handler for specific windows to appear in the main frame 
	void handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database) override;
	//updater for elements corresponding to specific screen
	void update() override;
	//function rendering screen
	void render(sf::RenderWindow& window) override;
	void renderToTexture(sf::RenderTexture& texture) override;

	GameCardState(Game* game);
};