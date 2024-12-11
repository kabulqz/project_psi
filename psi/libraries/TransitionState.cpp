#include "TransitionState.hpp"
#include "MainMenuState.hpp"
#include "GameBoardState.hpp"
#include "GameCardState.hpp"
#include "AbilityTreeState.hpp"

constexpr float TRANSITION_DURATION = 1.5f;

TransitionState::TransitionState(Game* game, State_enum targetState, std::unique_ptr<State> previusState) : game(game), targetState(targetState), previousState(std::move(previusState))
{
	if (!shader.loadFromFile("libraries/vhs_transition.frag", sf::Shader::Fragment))
	{
		std::cerr << "Cannot load shader\n";
	}

	shader.setUniform("progress", 0.0f);
	clock.restart();
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

void TransitionState::render(sf::RenderWindow& window)
{
	// Create the render texture for drawing the transition effects
	if (!renderTexture.create(window.getSize().x, window.getSize().y))
	{
		std::cerr << "Cannot create render texture\n";
		return;
	}

	// Create render textures for the "from" and "to" states
	sf::RenderTexture fromTexture;
	if (!fromTexture.create(window.getSize().x, window.getSize().y))
	{
		std::cerr << "Cannot create render texture\n";
		return;
	}

	// Render the previous state to the fromTexture
	previousState->renderToTexture(fromTexture);

	sf::RenderTexture toTexture;
	if (!toTexture.create(window.getSize().x, window.getSize().y))
	{
		std::cerr << "Cannot create render texture\n";
		return;
	}

	// Create a new state object based on the target state
	std::unique_ptr<State> tempState;
	switch (targetState)
	{
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

	// Render the "to" state to the toTexture
	tempState->renderToTexture(toTexture);

	// Set up shader uniforms
	shader.setUniform("fromTexture", fromTexture.getTexture());
	shader.setUniform("toTexture", toTexture.getTexture());

	// Pass the resolution of the window (or texture size) to the shader
	sf::Vector2u resolution = window.getSize();
	shader.setUniform("resolution", sf::Glsl::Vec2(resolution.x, resolution.y));

	// Calculate the progress of the transition
	float progress = clock.getElapsedTime().asSeconds() / TRANSITION_DURATION;

	if (progress > 1.0f)
	{
		// Once the transition is complete, change the state to the target state
		game->changeState(std::move(tempState));
		return;  // End the transition rendering
	}

	// Update the shader with the current progress of the transition
	shader.setUniform("progress", progress);

	// Clear and draw the transition
	window.clear();
	window.draw(sf::Sprite(fromTexture.getTexture()), &shader);  // Drawing the sprite with the shader applied
	window.display();
}

void TransitionState::renderToTexture(sf::RenderTexture& texture)
{
	State::renderToTexture(texture);
}