#include "State.hpp"

void State::renderToTexture(sf::RenderTexture& texture)
{
	// Clear the render texture
	texture.clear();
	// Draw the elements
	texture.display();
}