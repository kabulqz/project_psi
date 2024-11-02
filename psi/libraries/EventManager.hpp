#pragma once

#include "Settings.hpp"

class EventManager
{
private:
	std::queue<sf::Event> events;
public:
	void pushEvent(const sf::Event& event);
	bool hasEvents() const;
	sf::Event popEvent();
	void clear();
};