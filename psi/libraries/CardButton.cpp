#include "CardButton.hpp"

void CardButton::setPosition(const sf::Vector2i& pos)
{
	Button::setPosition({static_cast<float>(pos.x - 48), static_cast<float>(pos.y - 64)});
	card->setPosition({ static_cast<float>(pos.x - 48), static_cast<float>(pos.y - 64)});

	setUpCornerSprites(static_cast<int>(pos.x - 48), static_cast<int>(pos.y - 64));
	setUpBorderSprites(static_cast<int>(pos.x - 48), static_cast<int>(pos.y - 64));
}

CardButton::CardButton(Card* card, const sf::Vector2i& position, int elevation) :
Button(position.x, position.y, 96, 128, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
card(card), elevation(elevation)
{
	setPosition(position);
	setColor(sf::Color::Transparent);
}

void CardButton::displayDetails(sf::RenderTexture& renderTexture)
{
	card->displayDetails(renderTexture);
	Button::display(renderTexture);
}

void CardButton::display(sf::RenderTexture& renderTexture)
{
	card->display(renderTexture); 
	Button::display(renderTexture);
}

void CardButton::startDraggingCard(const sf::Vector2i& mousePos)
{
	isDragged = true;
	setPosition(mousePos);
}

void CardButton::stopDraggingCard(const sf::Vector2i& mousePos)
{
	isDragged = false;
	setPosition(mousePos);
}

void CardButton::updatePosition(const sf::Vector2i& mousePos)
{
	if (isDragged)
	{
		setPosition(mousePos);
	}
}

void CardButton::handleHoverState(const sf::Vector2i& mousePosition, sf::RenderTexture& renderTexture)
{
	if (isDragged) {
		hoverClock.restart();
		return;
	}

	if (isHovered(mousePosition))
	{
		if (hoverClock.getElapsedTime().asSeconds() > 1.0f) {// display enlarged and detailed card
			displayDetails(renderTexture);
		}
	}
	else {
		hoverClock.restart();
	}
}