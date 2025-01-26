#include "Settings.hpp"

#include "Button.hpp"
#include "Card.hpp"

class CardButton : public Button
{
protected:
	Card* card;
	bool isDragged = false;

	void setPosition(const sf::Vector2i& pos);
public:
	explicit CardButton(Card* card);
	~CardButton() = default;

	// update cards position
	void display(sf::RenderTexture& renderTexture);
	void startDraggingCard(const sf::Vector2i& mousePos);
	void stopDraggingCard(const sf::Vector2i& mousePos);
	void updatePosition(const sf::Vector2i& mousePos);
	bool isBeingDragged() const { return isDragged; }
	Card* getCard() const { return card; }
};