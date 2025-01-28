#include "Settings.hpp"

#include "Button.hpp"
#include "Card.hpp"

class CardButton : public Button
{
private:
	sf::Shader cardShader;
	sf::Texture cardTexture;
protected:
	Card* card;
	bool isDragged = false;
	int elevation;

	void displayDetails(sf::RenderTexture& renderTexture);
	void setPosition(const sf::Vector2i& pos);
public:
	CardButton(Card* card, const sf::Vector2i& position, int elevation);
	~CardButton() = default;

	// update cards position
	void display(sf::RenderTexture& renderTexture);
	void startDraggingCard(const sf::Vector2i& mousePos);
	void stopDraggingCard(const sf::Vector2i& mousePos);
	void updatePosition(const sf::Vector2i& mousePos);
	bool isBeingDragged() const { return isDragged; }
	Card* getCard() const { return card; }
	void handleHoverState(const sf::Vector2i& mousePosition, sf::RenderTexture& renderTexture);
	int getElevation() const { return elevation; }
	bool checkHoverDuration() const { return hoverClock.getElapsedTime().asSeconds() > 1.0f; }
	bool hasUpdatedEffects = false;
};