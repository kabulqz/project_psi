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

	if (!shader.loadFromFile("libraries/shader.frag", sf::Shader::Fragment))
	{
		std::cerr << "Cannot load shader from libraries/grayscale.frag\n";
	}
	else
	{
		std::cout << "Shader loaded successfully\n";
	}
}

void Ability::addChild(std::shared_ptr<Ability> child)
{
	children.push_back(child);
	childrenIds.push_back(child->id);
	child->parent = shared_from_this();
}

void Ability::display(sf::RenderWindow& window)
{
	switch (status)
	{
	case unlockStatus::UNLOCKED: // 0
		Button::setColor("2FBF71");
		window.draw(backgroundSprite);
		break;
	case unlockStatus::BUYABLE: // 1
		Button::setColor("F3CA40");
		window.draw(backgroundSprite, &shader);
		break;
	case unlockStatus::LOCKED: // 2
		Button::setColor("D90429");
		window.draw(backgroundSprite, &shader);
		break;
	}

	Button::display(window);
}

std::string Ability::serialize() const
{
	std::string status;
	if (this->status == unlockStatus::UNLOCKED) status = "UNLOCKED";
	else if (this->status == unlockStatus::BUYABLE) status = "BUYABLE";
	else if (this->status == unlockStatus::LOCKED) status = "LOCKED";

	// Serialize common data
	std::ostringstream ss;
	ss << id << ","					// Ability ID
		<< buyCost << ","			// Buy cost
		<< status << ","			// Status
		<< posX << ","				// Position X
		<< posY << ","				// Position Y		
		<< gridPosition.x << ","	// Grid position X
		<< gridPosition.y << ",";	// Grid position Y

	// Serialize the IDs of the children
	ss << "[";
	for (size_t i =0; i < children.size(); i++)
	{
		ss << children[i]->id;
		if (i < children.size() - 1)
		{
			ss << ",";
		}
	}
	ss << "]";

	return ss.str();
}

std::shared_ptr<Ability> Ability::deserialize(const std::string& serializedData)
{
	std::istringstream ss(serializedData);
	std::string token;
	int id, buyCost, posX, posY, gridX, gridY;
	std::string statusStr;
	unlockStatus status;
	std::vector<int> childrenIds;

	// Read common data
	std::getline(ss, token, ',');
	id = std::stoi(token);

	std::getline(ss, token, ',');
	buyCost = std::stoi(token);

	std::getline(ss, statusStr, ',');
	if (statusStr == "UNLOCKED") status = unlockStatus::UNLOCKED;
	else if (statusStr == "BUYABLE") status = unlockStatus::BUYABLE;
	else if (statusStr == "LOCKED") status = unlockStatus::LOCKED;

	std::getline(ss, token, ',');
	posX = std::stoi(token);
	std::getline(ss, token, ',');
	posY = std::stoi(token);

	std::getline(ss, token, ',');
	gridX = std::stoi(token);
	std::getline(ss, token, ',');
	gridY = std::stoi(token);

	// Create the ability
	auto ability = std::make_shared<Ability>(id, buyCost, status, posX, posY, sf::Vector2i(gridX, gridY));

	// Deserialize the children
	std::getline(ss, token, '['); // Skip the children part
	std::getline(ss, token, ']'); // Read the children IDs

	std::istringstream childrenStream(token);
	std::string childIdStr;

	while (std::getline(childrenStream, childIdStr, ','))
	{
		int childId = std::stoi(childIdStr);
		childrenIds.push_back(childId);
	}

	ability->setChildrenIds(childrenIds);

	return ability;
}

void AbilityTree::displayNode(std::shared_ptr<Ability> node, sf::RenderWindow& window, int depth)
{
	if (!node) return;

	node->display(window);

	for (const auto& child : node->getChildren())
	{
		displayNode(child, window, depth + 1);
	}
}

void AbilityTree::display(sf::RenderWindow& window) const
{
	displayNode(root, window, 0);
}

void AbilityTree::serializeNode(std::shared_ptr<Ability> node, std::ostringstream& oss)
{
	if (!node) return;

	oss << node->serialize() << ";";
	// Recursively serialize the children of this node
	for (const auto& child : node->getChildren())
	{
		serializeNode(child, oss);
	}
}

std::string AbilityTree::serialize() const
{
	std::ostringstream oss;
	oss << "{"; // Start of the tree
	serializeNode(root, oss);
	std::string result = oss.str();

	if (result.back() == ';')
	{
		result.pop_back(); // Remove the last semicolon
	}

	oss.str("");
	oss.clear();
	oss << result << "}"; // End of the tree

	return oss.str();
}

std::shared_ptr<AbilityTree> AbilityTree::deserialize(const std::string& data)
{
	std::istringstream iss(data);
	std::string line;
	std::unordered_map<int, std::shared_ptr<Ability>> nodeMap;
	std::shared_ptr<Ability> root = nullptr;

	// Remove the curly braces from the start and end
	if (data.front() == '{' && data.back() == '}') 
	{
		std::string strippedData = data.substr(1, data.size() - 2);
		std::istringstream dataStream(strippedData);

		// Split the serialized data by semicolons and deserialize each ability
		while (std::getline(dataStream, line, ';'))
		{
			auto ability = Ability::deserialize(line);
			if (ability)
			{
				nodeMap[ability->getId()] = ability;
			}
		}
	}

	// Now link the children to their parents
	for (const auto& pair : nodeMap)
	{
		auto ability = pair.second;

		// We need to link the children to the parent based on the children IDs
		for (int childId : ability->getChildrenIds())
		{
			if (nodeMap.find(childId) != nodeMap.end())
			{
				ability->addChild(nodeMap[childId]);
			}
		}

		// Find the root (the one without a parent)
		if (!ability->getParent()) 
		{
			root = ability;  // Assuming there is exactly one root in the tree
		}
	}

	return std::make_shared<AbilityTree>(root);
}