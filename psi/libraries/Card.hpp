#pragma once

#include "Settings.hpp"
#include "Effect.hpp"

class Card
{
	// Common attributes
	std::string back;
	std::string background;
	std::string portrait;
	std::string cardface;
	// List of keywords
	std::unordered_set<std::string> keywords;
	// Specific to Unit cards
	// List of statuses
	std::map<std::string, int> statuses;
	// List of effects
	std::vector<std::shared_ptr<Effect*>> effects;
	bool isOnBoard = false;
public:
	Card() = default;
	~Card() = default;

	// Methods to manage statuses
	void addStatus(const std::string& status);
	void removeStatus(const std::string& status);
	bool hasStatus(const std::string& status) const;
	void decrementStatuses();
	void addEffect(Effect* effect);
	void removeEffect(Effect* effect);

	void play();
};