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
	if (std::find(childrenIds.begin(), childrenIds.end(), child->id) == childrenIds.end())
	{
		childrenIds.push_back(child->id);
	}
	children.push_back(child);
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

std::string Ability::serialize() const
{
	std::ostringstream oss;

	oss << id << ","
		<< buyCost << ","
		<< (status == unlockStatus::UNLOCKED ? "UNLOCKED" : status == unlockStatus::BUYABLE ? "BUYABLE" : "LOCKED") << ","
		<< posX << ","
		<< posY << ","
		<< gridPosition.x << ","
		<< gridPosition.y;

	oss << ",[";
	for (size_t i =0; i<childrenIds.size(); ++i)
	{
		oss << childrenIds[i];
		if (i < childrenIds.size() - 1)
		{
			oss << ",";
		}
	}
	oss << "]";

	return oss.str();
}

std::shared_ptr<Ability> Ability::deserialize(const std::string& data)
{
	std::istringstream iss(data);
	std::string token;

	int id, buyCost, posX, posY, gridX, gridY;
	std::string statusStr;
	std::vector<int> childrenIds;

	std::getline(iss, token, ','); id = std::stoi(token);
	std::getline(iss, token, ','); buyCost = std::stoi(token);
	std::getline(iss, token, ','); statusStr = token;
	std::getline(iss, token, ','); posX = std::stoi(token);
	std::getline(iss, token, ','); posY = std::stoi(token);
	std::getline(iss, token, ','); gridX = std::stoi(token);
	std::getline(iss, token, ','); gridY = std::stoi(token);

	unlockStatus status;
	if (statusStr == "UNLOCKED") status = unlockStatus::UNLOCKED;
	else if (statusStr == "BUYABLE") status = unlockStatus::BUYABLE;
	else status = unlockStatus::LOCKED;

	std::getline(iss, token); // [...]
	if (token != "[]") // If there are children
	{
		token = token.substr(1, token.size() - 2); // Remove the brackets
		std::istringstream childStream(token);

		while (std::getline(childStream, token, ','))
		{
			childrenIds.push_back(std::stoi(token));
		}
	}

	auto ability = std::make_shared<Ability>(id, buyCost, status, posX, posY, sf::Vector2i(gridX, gridY));
	ability->setChildrenIds(childrenIds);

	return ability;
}

std::string AbilityTree::serialize() const
{
	if (!root) return "";

	std::ostringstream oss;
	std::function<void(const std::shared_ptr<Ability>&)> serializeNode;

	serializeNode = [&oss, &serializeNode](const std::shared_ptr<Ability>& node)
		{
			if (!node) return;

			oss << node->serialize() << ";";
			for (const auto& child : node->getChildren())
			{
				serializeNode(child);
			}
		};

	serializeNode(root);
	return oss.str();
}

std::shared_ptr<AbilityTree> AbilityTree::deserialize(const std::string& data)
{
	std::istringstream iss(data);
	std::string token;
	std::unordered_map<int, std::shared_ptr<Ability>> abilitiesById;

	// Step 1: Deserialize all abilities first and store them in a map by their ID 
	while (std::getline(iss, token, ';'))
	{
		auto ability = Ability::deserialize(token);
		abilitiesById[ability->getId()] = ability;
	}

	// Step 2: After all are deserialized, now link them by their parent-child relationships
	for (const auto& pair : abilitiesById)
	{
		auto ability = pair.second;

		// Set the parent for each child based on the childrenIds
		for (int childId : ability->getChildrenIds())
		{
			if (abilitiesById.contains(childId))
			{
				auto child = abilitiesById[childId];
				ability->addChild(child);
			}
		}
	}

	// Step 3: Find the root (ability with no parent)
	std::shared_ptr<Ability> root = nullptr;
	for (const auto& pair : abilitiesById)
	{
		if (pair.second->getParent() == nullptr)
		{
			root = pair.second;
			break;
		}
	}

	// Step 4: Return the tree
	return std::make_shared<AbilityTree>(root);
}

std::shared_ptr<AbilityTree> AbilityTreeFactory::createAbilityTree()
{
	// Create abilities
	auto startAbility = std::make_shared<Ability>(0, 1, Ability::unlockStatus::UNLOCKED, 616, 605, sf::Vector2i(12, 2));
	auto heal1 = std::make_shared<Ability>(1, 1, Ability::unlockStatus::LOCKED, 617, 510, sf::Vector2i(9, 8));
	auto heal2 = std::make_shared<Ability>(4, 1, Ability::unlockStatus::LOCKED, 621, 403, sf::Vector2i(10, 8));
	auto heal3 = std::make_shared<Ability>(13, 1, Ability::unlockStatus::LOCKED, 620, 300, sf::Vector2i(11, 8));
	auto heal4 = std::make_shared<Ability>(17, 1, Ability::unlockStatus::LOCKED, 620, 188, sf::Vector2i(12, 8));
	auto fireball1 = std::make_shared<Ability>(5, 1, Ability::unlockStatus::LOCKED, 498, 392, sf::Vector2i(6, 0));
	auto fireball2 = std::make_shared<Ability>(15, 1, Ability::unlockStatus::LOCKED, 703, 244, sf::Vector2i(5, 0));
	auto freeze1 = std::make_shared<Ability>(7, 1, Ability::unlockStatus::LOCKED, 548, 337, sf::Vector2i(11, 6));
	auto freeze2 = std::make_shared<Ability>(12, 1, Ability::unlockStatus::LOCKED, 805, 250, sf::Vector2i(12, 6));
	auto plusSpeed1 = std::make_shared<Ability>(3, 1, Ability::unlockStatus::LOCKED, 694, 438, sf::Vector2i(14, 8));
	auto plusSpeed2 = std::make_shared<Ability>(8, 1, Ability::unlockStatus::LOCKED, 434, 345, sf::Vector2i(0, 9));
	auto buffMeleeUnits1 = std::make_shared<Ability>(2, 1, Ability::unlockStatus::LOCKED, 553, 443, sf::Vector2i(4, 9));
	auto buffMeleeUnits2 = std::make_shared<Ability>(11, 1, Ability::unlockStatus::LOCKED, 815, 347, sf::Vector2i(6, 9));
	auto buffMeleeUnits3 = std::make_shared<Ability>(19, 1, Ability::unlockStatus::LOCKED, 656, 101, sf::Vector2i(5, 9));
	auto buffRangedUnits = std::make_shared<Ability>(16, 1, Ability::unlockStatus::LOCKED, 774, 192, sf::Vector2i(8, 9));
	auto maxCardHand = std::make_shared<Ability>(9, 1, Ability::unlockStatus::LOCKED, 468, 269, sf::Vector2i(9, 9));
	auto maxHealth1 = std::make_shared<Ability>(10, 1, Ability::unlockStatus::LOCKED, 706, 331, sf::Vector2i(10, 9));
	auto maxHealth2 = std::make_shared<Ability>(14, 1, Ability::unlockStatus::LOCKED, 538, 218, sf::Vector2i(11, 9));
	auto maxEnergy1 = std::make_shared<Ability>(6, 1, Ability::unlockStatus::LOCKED, 755, 392, sf::Vector2i(3, 7));
	auto maxEnergy2 = std::make_shared<Ability>(18, 1, Ability::unlockStatus::LOCKED, 584, 101, sf::Vector2i(2, 7));

	// Setting up the tree
	startAbility->addChild(heal1);
	heal1->addChild(buffMeleeUnits1);
	heal1->addChild(plusSpeed1);
	heal1->addChild(heal2);
	heal2->addChild(fireball1);
	heal2->addChild(maxEnergy1);
	heal2->addChild(freeze1);
	freeze1->addChild(maxCardHand);
	freeze1->addChild(plusSpeed2);
	heal2->addChild(maxHealth1);
	maxHealth1->addChild(buffMeleeUnits2);
	maxHealth1->addChild(freeze2);
	heal2->addChild(heal3);
	heal3->addChild(maxHealth2);
	heal3->addChild(fireball2);
	fireball2->addChild(buffRangedUnits);
	heal3->addChild(heal4);
	heal4->addChild(buffMeleeUnits3);
	heal4->addChild(maxEnergy2);

	return std::make_shared<AbilityTree>(startAbility);
}