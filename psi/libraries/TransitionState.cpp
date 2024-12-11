#include "TransitionState.hpp"
#include "MainMenuState.hpp"
#include "GameBoardState.hpp"
#include "GameCardState.hpp"
#include "AbilityTreeState.hpp"

constexpr float TRANSITION_DURATION = 1.15f;

TransitionState::TransitionState(Game* game, State_enum targetState, std::unique_ptr<State> previousState) : game(game), targetState(targetState), previousState(std::move(previousState))
{
	if (!shader.loadFromFile("libraries/vhs_transition.frag", sf::Shader::Fragment))
	{
		std::cerr << "Cannot load shader\n";
	}

	shader.setUniform("progress", 0.0f);
	clock.restart();

	// Create the render texture for the "from" state
	if (!fromTexture.create(game->getWindow().getSize().x, game->getWindow().getSize().y)) {
		std::cerr << "Cannot create render texture for from state\n";
		return;
	}
	this->previousState->renderToTexture(fromTexture);  // Render the previous state to texture

	// Create the render texture for the "to" state
	if (!toTexture.create(game->getWindow().getSize().x, game->getWindow().getSize().y)) {
		std::cerr << "Cannot create render texture for to state\n";
		return;
	}

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
}

void TransitionState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager,
	sqlite3*& database)
{
	// Nothing to see here
	// This state is only transition and providing animation between states
	// No input is needed
}

void TransitionState::update()
{
	
}

void TransitionState::render(sf::RenderWindow& window) {
	float progress = clock.getElapsedTime().asSeconds() / TRANSITION_DURATION;

	if (progress > TRANSITION_DURATION) {
		// Once the transition is complete, change the state to the target state
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
	shader.setUniform("progress", progress);
	shader.setUniform("fromTexture", fromTexture.getTexture());
	shader.setUniform("toTexture", toTexture.getTexture());
	shader.setUniform("resolution", sf::Vector2f(window.getSize().x, window.getSize().y));

	// Clear the window and draw the transition effect
	window.clear();
	window.draw(sf::Sprite(fromTexture.getTexture()), &shader);  // Drawing the sprite with the shader applied
	window.display();
}

void TransitionState::renderToTexture(sf::RenderTexture& texture)
{
	State::renderToTexture(texture);
}