#include "GameCardState.hpp"
#include "Game.hpp"

GameCardState::GameCardState(Game* game) : game(game)
{

}

//handler for specific windows to appear in the main frame 
void GameCardState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	if (!soundManager.isSoundPlaying("Ambience_crt"))
	{
		soundManager.playSound("Ambience_crt");
	}

	while (eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
	}
}

//updater for elements corresponding to specific screen
void GameCardState::update()
{

}

//function rendering screen
void GameCardState::render(sf::RenderWindow& window)
{
	window.clear();
	//draw elements
#ifdef _DEBUG

#else

#endif
	window.display();
}

void GameCardState::renderToTexture(sf::RenderTexture& texture)
{
	// Clear the render texture
	texture.clear();
	// Draw the elements
	texture.display();
}
