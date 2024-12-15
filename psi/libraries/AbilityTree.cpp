#include "AbilityTree.hpp"

Ability::Ability(const Ability& ability) : Ability(ability.id, ability.buyCost, ability.status, ability.posX, ability.posY, ability.gridPosition){}

Ability::Ability(const int& id, const int& buyCost, const unlockStatus& status, const int& pos_x, const int& pos_y, const sf::Vector2i& gridPosition) : Button(pos_x, pos_y, 48, 48, "src/img/borders/panel-border-013.png")
{
	this->id = id;
	this->buyCost = buyCost;
	this->status = status;
	this->posX = pos_x;
	this->posY = pos_y;
	this->gridPosition = gridPosition;
	const std::string backgroundPath = "src/img/abilities.png";

	//setting the background image
	sf::IntRect textureRect{
		gridPosition.x * 48,	// Left
		gridPosition.y * 48,	// Top
		48,						// Width
		48						// Height
	};
	if (!backgroundTexture.loadFromFile(backgroundPath))
	{
		std::cerr << "Cannot load background from " << backgroundPath << "\n";
		return;
	}
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setTextureRect(textureRect); // Crop the texture
	backgroundSprite.setPosition(static_cast<float>(pos_x), static_cast<float>(pos_y));
	backgroundSprite.setScale(static_cast<float>(getWidth()) / 48, static_cast<float>(getHeight()) / 48);
}

void Ability::addChild(std::shared_ptr<Ability> child)
{
	children.push_back(child);
	childrenIds.push_back(child->id);
	child->parent = shared_from_this();
}

void Ability::display(sf::RenderTarget& window, const sf::Shader* shader)
{
	switch (status)
	{
	case unlockStatus::UNLOCKED: // 0
		Button::setColor("2FBF71");
		window.draw(backgroundSprite);
		break;
	case unlockStatus::BUYABLE: // 1
		Button::setColor("F3CA40");
		window.draw(backgroundSprite, shader);
		break;
	case unlockStatus::LOCKED: // 2
		Button::setColor("D90429");
		window.draw(backgroundSprite, shader);
		break;
	}

	Button::display(window);
}

AbilityTree::AbilityTree(const std::shared_ptr<Ability>& root) : root(root)
{
	if (!shader.loadFromFile("libraries/grayscale.frag", sf::Shader::Fragment))
	{
		std::cerr << "Cannot load shader from libraries/grayscale.frag\n";
	}
	else
	{
		std::cout << "Shader loaded successfully\n";
	}
}

void AbilityTree::displayNode(const std::shared_ptr<Ability>& node, sf::RenderTarget& window, const sf::Shader* shader, const int depth)
{
	if (!node) return;

	node->display(window, shader);

	for (const auto& child : node->getChildren())
	{
		displayNode(child, window, shader, depth + 1);
	}
}

void AbilityTree::display(sf::RenderTarget& window) const
{
	displayNode(root, window, &shader, 0);
}