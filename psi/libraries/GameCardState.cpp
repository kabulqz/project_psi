#include "GameCardState.hpp"
#include "Game.hpp"

void GameCardState::adjustEffectDescriptionSize(Button& effectDescription) const
{
	sf::Text tempText;
	sf::Font tempFont;
	tempFont.loadFromFile(game->getSettings().getFont());
	tempText.setFont(tempFont);
	tempText.setCharacterSize(game->getSettings().getFontSize() - 8);

	tempText.setString(effectDescription.getText());
	const sf::FloatRect textBounds = tempText.getLocalBounds();

	const int buttonWidth = textBounds.width + 20;
	const int buttonHeight = textBounds.height + 20;

	effectDescription.resizeTo(buttonWidth, buttonHeight);
}

void GameCardState::adjustEffectDescriptionsLayout() const
{
	constexpr int margin = 10;
	sf::Vector2f currentPosition(margin, 720 - margin);

	for (const auto& effectDescription : effectDescriptions)
	{
		adjustEffectDescriptionSize(*effectDescription);

		const float buttonWidth = static_cast<float>(effectDescription->getWidth());
		const float buttonHeight = static_cast<float>(effectDescription->getHeight());
		//std::cout << "Button size: " << buttonWidth << "x" << buttonHeight << "\n";

		currentPosition.y -= buttonHeight;
		effectDescription->setPosition(currentPosition);

		//std::cout << "Set position to: (" << currentPosition.x << ", " << currentPosition.y << ")" << "\n";

		currentPosition.y -= margin;
	}
}

GameCardState::GameCardState(Game* game) : game(game),
enemyHand(160, 10, 960, 160, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
playerHand(160, 550, 960, 160, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
enemyBattlefield(140, 175, 1000, 180, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
playerBattlefield(140, 365, 1000, 180, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
 enemyDeck(1152, 154, 96, 128, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
playerDeck(1152, 438, 96, 128, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
PASS(10, 320, 120, 80, PATH_TO_BORDERS_FOLDER + "panel-border-031.png"),
effectDescription1(10, 10, 10, 10, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
effectDescription2(10, 10, 10, 10, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
effectDescription3(10, 10, 10, 10, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
effectDescription4(10, 10, 10, 10, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
effectDescription5(10, 10, 10, 10, PATH_TO_BORDERS_FOLDER + "panel-border-030.png"),
effectDescription6(10, 10, 10, 10, PATH_TO_BORDERS_FOLDER + "panel-border-030.png")
{
	sf::Image cursorDefault;
	sf::Image cursorOpenHand;
	sf::Image cursorClosedHand;
	sf::Image cursorDetails;
	const std::string cursorDefaultPath = "src/img/cursors/";
	if (!cursorDefault.loadFromFile(cursorDefaultPath + "cursor_default.png"))
	{
		std::cerr << "Cannot load cursor_default.png\n";
	}
	if (!cursorOpenHand.loadFromFile(cursorDefaultPath + "cursor_open_hand.png"))
	{
		std::cerr << "Cannot load cursor_open_hand.png\n";
	}
	if (!cursorClosedHand.loadFromFile(cursorDefaultPath + "cursor_closed_hand.png"))
	{
		std::cerr << "Cannot load cursor_closed_hand.png\n";
	}
	if (!cursorDetails.loadFromFile(cursorDefaultPath + "cursor_details.png"))
	{
		std::cerr << "Cannot load cursor_details.png\n";
	}
	defaultCursor.loadFromPixels(cursorDefault.getPixelsPtr(), cursorDefault.getSize(), { 0, 0 });
	openHandCursor.loadFromPixels(cursorOpenHand.getPixelsPtr(), cursorOpenHand.getSize(), { 16, 16 });
	closedHandCursor.loadFromPixels(cursorClosedHand.getPixelsPtr(), cursorClosedHand.getSize(), { 16, 16 });
	detailsCursor.loadFromPixels(cursorDetails.getPixelsPtr(), cursorDetails.getSize(), { 16, 16 });

	playerHand.setText("Player's Hand", game->getSettings().getFont(), game->getSettings().getFontSize());
	playerBattlefield.setText("Player's Battlefield", game->getSettings().getFont(), game->getSettings().getFontSize());
	enemyHand.setText("Enemy's Hand", game->getSettings().getFont(), game->getSettings().getFontSize());
	enemyBattlefield.setText("Enemy's Battlefield", game->getSettings().getFont(), game->getSettings().getFontSize());

	PASS.setBackgroundColor("000000");
	PASS.setText("PASS", game->getSettings().getFont(), game->getSettings().getFontSize());
	PASS.setEnabled(false);

	std::random_device rd;
	// Load the save and create the player and enemy instances
	save = game->getSave();
	game->changeViewZoom(1.0f);
	player = new Hero();
	enemy = new Hero();
	player->getDeck().clear();
	enemy->getDeck().clear();

	// copy deck from save to player
	player->copyDeck(save->getDeck());
	// create for-loop to create enemy deck
	std::mt19937 gen(rd());
	std::uniform_int_distribution cardSeedDistribution(std::mt19937::min(), std::mt19937::max());
	std::vector<uint_least32_t> tempEnemyDeck;
	tempEnemyDeck.reserve(30);
	for (int i = 0; i < 30; i++) {
		tempEnemyDeck.push_back(cardSeedDistribution(gen));
	}
	// copy deck to enemy
	enemy->copyDeck(tempEnemyDeck);
	// set hero cards enemy and hero by method setOwnerAndEnemy(player, enemy)
	for (const auto card : player->getDeck())
	{
		card->setOwnerAndEnemy(player, enemy);
	}
	// and the other way around but use setOwnerAndEnemy(enemy, player)
	for (const auto card : enemy->getDeck())
	{
		card->setOwnerAndEnemy(enemy, player);
	}

	std::uniform_int_distribution<int> backgroundDistribution(0, 19);
	const int choice = backgroundDistribution(rd);
	chosenBackground = backgrounds[choice];

	if (!backgroundShader.loadFromFile("libraries/background.frag", sf::Shader::Fragment)) return;
	if (!vhsShader.loadFromFile("libraries/vhs_effect.frag", sf::Shader::Fragment)) return;
	shaderClock.restart();

	int elevation = 0;
	for (int i = 0; i < player->getDeck().size(); i++)
	{
		sf::Vector2i playerDeckPosition(1152 + 48, 438 + 64);
		playerCardButtons.push_back(new CardButton(player->getDeck()[i], playerDeckPosition, elevation));
		elevation++;
		playerCardButtons.back()->getCard()->setFontAndFontSize(game->getSettings().getFont(), game->getSettings().getFontSize() - 5);
		playerCardButtons.back()->getCard()->flip();
	}
	elevation = 0;
	for (int i = 0; i < enemy->getDeck().size(); i++)
	{
		sf::Vector2i enemyDeckPosition(1152 + 48, 154 + 64);
		enemyCardButtons.push_back(new CardButton(enemy->getDeck()[i], enemyDeckPosition, elevation));
		elevation++;
		enemyCardButtons.back()->getCard()->setFontAndFontSize(game->getSettings().getFont(), game->getSettings().getFontSize() - 5);
	}
}

//handler for specific windows to appear in the main frame 
void GameCardState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	if (!soundManager.isSoundPlaying("Ambience_crt"))
	{
		soundManager.playSound("Ambience_crt");
	}

	mousePos = sf::Mouse::getPosition(window);
	CardButton* topCard = nullptr;
	int highestElevation = -1;

	for (const auto& cardButton : playerCardButtons)
	{
		if (cardButton->getElevation() > highestElevation && cardButton->isHovered(mousePos))
		{
			highestElevation = cardButton->getElevation();
			topCard = cardButton;
		}

		if (topCard && topCard->isBeingDragged())
		{
			topCard->updatePosition(mousePos);
		}
	}

	while (eventManager.hasEvents())
	{
		const sf::Event event = eventManager.popEvent();
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C)
		{// GO TO CARD GAME
			save->write();
			game->setSave(save);

			game->changeState(std::make_unique<TransitionState>(game, GAME_CARD, GAME_BOARD));
			break;
		}
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) // RELOAD BACKGROUND
		{
			std::random_device rd;
			std::uniform_int_distribution<int> backgroundDistribution(0, 19);
			const int choice = backgroundDistribution(rd);
			chosenBackground = backgrounds[choice];
			std::cout << color("B2FFD6", "chose background number " + std::to_string(choice + 1) + "\n");
		}
		if (PASS.isClickable() && PASS.isHovered(mousePos))
		{
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
			{
				// Pass the turn
				// logic for passing the turn
				// simulating the enemy turn
			}
		}

		for (const auto cardButton : playerCardButtons)
		{
			if (cardButton->isClickable() && cardButton->isHovered(mousePos))
			{
				if (topCard != nullptr && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
				{
					topCard->getCard()->flip();
				}
				if (topCard != nullptr && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
				{
					topCard->startDraggingCard(mousePos);
				}
				if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
				{
					//cardButton->getCard()->flip();
					cardButton->stopDraggingCard(mousePos);
				}
			}
		}
	}
}

void GameCardState::updateCardEffects(const CardButton* cardButton) const
{
	const auto effects = cardButton->getCard()->getEffects();
	size_t effectIndex = 0;
	for (auto& effectDescription : effectDescriptions)
	{
		// Ustawienie efektów dla danej karty, tylko jeśli efekt istnieje
		if (effectIndex < effects.size() && !effects[effectIndex].empty())
		{
			effectDescription->setBackgroundColor("000000");
			effectDescription->setText(effects[effectIndex], game->getSettings().getFont(), game->getSettings().getFontSize() - 8);
			effectDescription->setVisible(true);  // Ustawienie widoczności na true
			//adjustEffectDescriptionSize(*effectDescription);
		}
		else
		{
			effectDescription->setBackgroundColor("000000");
			effectDescription->setText("", game->getSettings().getFont(), game->getSettings().getFontSize() - 8);
			effectDescription->setVisible(false);  // Ukrywamy opis efektu
		}
		effectIndex++;
	}
	adjustEffectDescriptionsLayout();
	adjustEffectDescriptionsLayout(); // dla poprawki
}

//updater for elements corresponding to specific screen
void GameCardState::update()
{
	float elapsedTime = shaderClock.getElapsedTime().asSeconds();

	// Update VHS shader
	vhsShader.setUniform("time", elapsedTime);
	vhsShader.setUniform("resolution", sf::Vector2f(1280, 720));

	// Update background shader
	backgroundShader.setUniform("time", elapsedTime);
	backgroundShader.setUniform("spin_time", elapsedTime * 0.4f);
	backgroundShader.setUniform("colour_1", chosenBackground.colour1); // Adjust colors as needed
	backgroundShader.setUniform("colour_2", chosenBackground.colour2);
	backgroundShader.setUniform("colour_3", chosenBackground.colour3);
	backgroundShader.setUniform("contrast", 1.0f);
	backgroundShader.setUniform("spin_amount", 0.7f);
	backgroundShader.setUniform("screenSize", sf::Vector2f(1280.0f, 720.0f));

	for (auto& cardButton : playerCardButtons)
	{
		if (showingEffects == false && cardButton->checkHoverDuration() && !cardButton->hasUpdatedEffects)
		{
			updateCardEffects(cardButton);
			cardButton->hasUpdatedEffects = true;  // Ustawiamy flagę, żeby nie wywoływać tej funkcji ponownie
			showingEffects = true;
		}
		else if (!cardButton->checkHoverDuration())
		{
			cardButton->hasUpdatedEffects = false;  // Resetujemy flagę, gdy karta przestaje być hoverowana
		}
		else
		{
			showingEffects = false;
		}
	}
}

void GameCardState::handleCursorChange(
	sf::RenderWindow& window,
	CardButton& cardButton,
	bool& isHoveredOverAnyCard,
	CursorState& cursorState) const
{
	// Sprawdź, czy przycisk karty jest najechany
	if (cardButton.isHovered(mousePos))
	{
		isHoveredOverAnyCard = true;

		// Ustal nowy stan kursora w oparciu o warunki
		CursorState newCursorState = {};
		if (cardButton.isBeingDragged() && !cardButton.checkHoverDuration())
		{
			newCursorState = CursorState::ClosedHand;
		}
		else if (!cardButton.isBeingDragged() && !cardButton.checkHoverDuration())
		{
			newCursorState = CursorState::OpenHand;
		}
		else if (!cardButton.isBeingDragged() && cardButton.checkHoverDuration())
		{
			newCursorState = CursorState::Details;
		}

		// Zmień kursor, jeśli stan kursora uległ zmianie
		if (newCursorState != cursorState)
		{
			cursorState = newCursorState;
			switch (cursorState)
			{
			case CursorState::ClosedHand:
				window.setMouseCursor(closedHandCursor);
				break;
			case CursorState::OpenHand:
				window.setMouseCursor(openHandCursor);
				break;
			case CursorState::Details:
				window.setMouseCursor(detailsCursor);
				break;
			default:
				window.setMouseCursor(defaultCursor);
				break;
			}
		}
	}
}

//function rendering screen
void GameCardState::render(sf::RenderWindow& window)
{
	sf::RenderTexture renderTexture;
	if (!renderTexture.create(window.getSize().x, window.getSize().y)) {
		std::cerr << "Cannot create render texture\n";
		return;
	}
	renderTexture.clear(sf::Color::Transparent);
	renderTexture.setView(game->getView());

	sf::RectangleShape fullScreenQuad(sf::Vector2f(window.getSize().x, window.getSize().y));
	//renderTexture.draw(background);
	renderTexture.draw(fullScreenQuad, &backgroundShader);

	playerHand.display(renderTexture);
	playerBattlefield.display(renderTexture);
	playerDeck.display(renderTexture);
	enemyHand.display(renderTexture);
	enemyBattlefield.display(renderTexture);
	enemyDeck.display(renderTexture);
	PASS.display(renderTexture);

	for (auto cardButton : enemyCardButtons)
	{
		cardButton->display(renderTexture);
		cardButton->handleHoverState(mousePos, renderTexture);
	}

	for (auto& cardButton : playerCardButtons)
	{
		cardButton->display(renderTexture);
		cardButton->handleHoverState(mousePos, renderTexture);
	}

	bool isHoveredOverAnyCard = false;
	CursorState currentCursorState = CursorState::Default;

	for (auto& cardButton : playerCardButtons)
	{
		handleCursorChange(window, *cardButton, isHoveredOverAnyCard, currentCursorState);
	}

	if (!isHoveredOverAnyCard)
	{
		window.setMouseCursor(defaultCursor);
	}

	for (auto& cardButton : playerCardButtons)
	{
		if (cardButton->hasUpdatedEffects)
		{
			for (auto& effectDescription : effectDescriptions)
			{
				effectDescription->display(renderTexture);
			}
		}
	}

	renderTexture.display();
	window.clear();
	sf::Sprite screenSprite(renderTexture.getTexture());

#ifdef _DEBUG
	window.draw(screenSprite);
#else
	window.draw(screenSprite, &vhsShader);
#endif
	window.display();
}

void GameCardState::renderToTexture(sf::RenderTexture& texture)
{
	// Clear the render texture
	texture.clear();
	texture.setView(game->getView());

	sf::RectangleShape fullScreenQuad(sf::Vector2f(1280, 720));
	//renderTexture.draw(background);
	backgroundShader.setUniform("time", shaderClock.getElapsedTime().asSeconds());
	backgroundShader.setUniform("spin_time", shaderClock.getElapsedTime().asSeconds() * 0.4f);
	backgroundShader.setUniform("colour_1", chosenBackground.colour1); // Adjust colors as needed
	backgroundShader.setUniform("colour_2", chosenBackground.colour2);
	backgroundShader.setUniform("colour_3", chosenBackground.colour3);
	backgroundShader.setUniform("contrast", 1.0f);
	backgroundShader.setUniform("spin_amount", 0.7f);
	backgroundShader.setUniform("screenSize", sf::Vector2f(1280.0f, 720.0f));
	texture.draw(fullScreenQuad, &backgroundShader);

	playerHand.display(texture);
	playerBattlefield.display(texture);
	playerDeck.display(texture);
	enemyHand.display(texture);
	enemyBattlefield.display(texture);
	enemyDeck.display(texture);
	PASS.display(texture);

	// Draw the elements
	texture.display();
}