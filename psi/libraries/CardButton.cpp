#include "CardButton.hpp"

void CardButton::setPosition(const sf::Vector2i& pos)
{
	Button::setPosition({static_cast<float>(pos.x - 48), static_cast<float>(pos.y - 64)});
	card->setPosition({ static_cast<float>(pos.x - 48), static_cast<float>(pos.y - 64)});
}

CardButton::CardButton(Card* card) :
Button(0, 0, 96, 128, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
card(card)
{
	setColor(sf::Color::Transparent);
}

void CardButton::display(sf::RenderTexture& renderTexture)
{
	Button::display(renderTexture);
	card->display(renderTexture);
}

void CardButton::startDraggingCard(const sf::Vector2i& mousePos)
{
	isDragged = true;
	setPosition(mousePos);
}

void CardButton::stopDraggingCard(const sf::Vector2i& mousePos)
{
	isDragged = false;
}

void CardButton::updatePosition(const sf::Vector2i& mousePos)
{
	if (isDragged)
	{
		setPosition(mousePos);
	}
}
