#include "TransitionState.hpp"

#include "MainMenuState.hpp"
#include "GameBoardState.hpp"
#include "GameCardState.hpp"
#include "AbilityTreeState.hpp"

constexpr float TRANSITION_DURATION = 2.0f;

TransitionState::TransitionState(Game* game, State_enum targetState, std::unique_ptr<State> previousState) : game(game), targetState(targetState), previousState(std::move(previousState))
{
	sf::Image cursorDefault;
	sf::Image cursorLoading;

	const std::string cursorDefaultPath = "src/img/cursors/";

	if (!cursorDefault.loadFromFile(cursorDefaultPath + "cursor_default.png") || !cursorLoading.loadFromFile(cursorDefaultPath + "cursor_loading.png"))
	{
		std::cerr << "Cannot load cursors png files\n";
	}

	defaultCursor.loadFromPixels(cursorDefault.getPixelsPtr(), cursorDefault.getSize(), { 0, 0 });
	loadingCursor.loadFromPixels(cursorLoading.getPixelsPtr(), cursorLoading.getSize(), { 16, 16 });

	if (!transitionShader.loadFromFile("libraries/vhs_transition.frag", sf::Shader::Fragment))
	{
		std::cerr << "Cannot load shader\n";
	}

	transitionShader.setUniform("progress", 0.0f);
	transitionClock.restart();

	if (!vhsShader.loadFromFile("libraries/vhs_effect.frag", sf::Shader::Fragment))
	{
		std::cerr << "Cannot load shader\n";
	}

	// Create the render texture for the "from" state
	if (!fromTexture.create(game->getWindow().getSize().x, game->getWindow().getSize().y)) return;
	this->previousState->renderToTexture(fromTexture);  // Render the previous state to texture


	// Create the render texture for the "to" state
	if (!toTexture.create(game->getWindow().getSize().x, game->getWindow().getSize().y)) return;

	// Render the target state to texture
	std::unique_ptr<State> tempState;
	switch (targetState) {
	case MAIN_MENU:
		tempState = std::make_unique<MainMenuState>(game);
		break;
	case GAME_BOARD:
		tempState = std::make_unique<GameBoardState>(game);
		break;
	case GAME_CARD:
		tempState = std::make_unique<GameCardState>(game);
		break;
	case ABILITY_TREE:
		tempState = std::make_unique<AbilityTreeState>(game);
		break;
	default:
		tempState = std::make_unique<MainMenuState>(game);
		break;
	}
	tempState->renderToTexture(toTexture);  // Render the target state to texture

	float progress = transitionClock.getElapsedTime().asSeconds();

	vhsShader.setUniform("time", progress);
	vhsShader.setUniform("resolution", sf::Vector2f(1280, 720));

	if (!vhsTextureFrom.create(fromTexture.getSize().x, fromTexture.getSize().y))
	{
		std::cerr << "Cannot create render texture for vhs effect\n";
		return;
	}
	vhsTextureFrom.clear();
	vhsTextureFrom.draw(sf::Sprite(fromTexture.getTexture()), &vhsShader);
	vhsTextureFrom.display();

	if (!vhsTextureTo.create(toTexture.getSize().x, toTexture.getSize().y))
	{
		std::cerr << "Cannot create render texture for vhs effect\n";
		return;
	}
	vhsTextureTo.clear();
	vhsTextureTo.draw(sf::Sprite(toTexture.getTexture()), &vhsShader);
	vhsTextureTo.display();
}

void TransitionState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager,
	sqlite3*& database)
{
	if (!soundManager.isSoundPlaying("Ambience_crt"))
	{
		soundManager.playSound("Ambience_crt");
	}

	// Nothing to see here
	// This state is only transition and providing animation between states
	// No input is needed
}

void TransitionState::update()
{
	// Nothing to see here
	// This state is only transition and providing animation between states
	// No update is needed
}

void TransitionState::render(sf::RenderWindow& window)
{
	window.setMouseCursor(loadingCursor);

	float progress = transitionClock.getElapsedTime().asSeconds() / TRANSITION_DURATION;
	progress = std::min(progress, 1.0f);

	if (progress >= 1.0f) {
		// Once the transition is complete, change the state to the target state
		window.setMouseCursor(defaultCursor);
		switch (targetState) {
		case MAIN_MENU:
			game->changeState(std::make_unique<MainMenuState>(game));
			break;
		case GAME_BOARD:
			game->changeState(std::make_unique<GameBoardState>(game));
			break;
		case GAME_CARD:
			game->changeState(std::make_unique<GameCardState>(game));
			break;
		case ABILITY_TREE:
			game->changeState(std::make_unique<AbilityTreeState>(game));
			break;
		}
		return;  // End the transition rendering
	}

	// Smooth the progress with easing (for smooth transition effect)
	progress = 3 * progress * progress - 2 * progress * progress * progress;  // cubic easing
	transitionShader.setUniform("progress", progress);
	transitionShader.setUniform("fromTexture", vhsTextureFrom.getTexture());
	transitionShader.setUniform("toTexture", vhsTextureTo.getTexture());
	transitionShader.setUniform("resolution", sf::Vector2f(window.getSize().x, window.getSize().y));

	// Clear the window and draw the transition effect
	window.clear();
	window.draw(sf::Sprite(vhsTextureFrom.getTexture()), &transitionShader);  // Drawing the sprite with the shader applied
	window.display();
}

void TransitionState::renderToTexture(sf::RenderTexture& texture)
{
	State::renderToTexture(texture);
}