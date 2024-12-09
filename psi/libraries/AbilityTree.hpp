#pragma once

#include "Settings.hpp"
#include "Button.hpp"
#include "Save.hpp"

class Ability : public Button, public std::enable_shared_from_this<Ability>
{
public:
	enum class unlockStatus { UNLOCKED, BUYABLE, LOCKED };
private:
	int id;
	int buyCost;
	unlockStatus status = unlockStatus::LOCKED;

	std::vector<std::shared_ptr<Ability>> children;
	std::shared_ptr<Ability> parent;
	std::vector<int> childrenIds;

	// Graphics properties
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	int posX;
	int posY;
	sf::Vector2i gridPosition;
public:
	Ability(const Ability& ability);
	Ability(const int& id, const int& buyCost, const unlockStatus& status, const int& pos_x, const int& pos_y, const sf::Vector2i& gridPosition);

	void addChild(std::shared_ptr<Ability> child);
	void setParent(std::shared_ptr<Ability> parent) { this->parent = parent; }
	std::shared_ptr<Ability> getParent() const { return parent; }
	const std::vector<std::shared_ptr<Ability>>& getChildren() const { return children; }
	unlockStatus getUnlockStatus() const { return status; }
	void setUnlockStatus(unlockStatus status) { this->status = status; }
	int getBuyCost() const { return buyCost; }
	void setBuyCost(int buyCost) { this->buyCost = buyCost; }
	int getId() const { return id; }
	// Display the ability on the screen
	void display(sf::RenderWindow& window, const sf::Shader* shader = nullptr);
	void setChildrenIds(const std::vector<int>& ids) { childrenIds = ids; }
	std::vector<int>& getChildrenIds() { return childrenIds; }


	//Serialization and deserialization
	std::string serialize() const;
	static std::shared_ptr<Ability> deserialize(const std::string& data);
};

class AbilityTree
{
private:
	std::shared_ptr<Ability> root;
	static void displayNode(const std::shared_ptr<Ability>& node, sf::RenderWindow& window, const sf::Shader* shader, int depth);
	static void serializeNode(const std::shared_ptr<Ability>& node, std::ostringstream& ss);
	sf::Shader shader;
public:
	AbilityTree(const std::shared_ptr<Ability>& root);

	std::shared_ptr<Ability> getRoot() { return root; }
	sf::Shader* getShader() { return &shader; }
	void display(sf::RenderWindow& window) const;

	std::string serialize() const;
	static std::shared_ptr<AbilityTree> deserialize(const std::string& data);
};