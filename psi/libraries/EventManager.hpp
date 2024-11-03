#pragma once

#include "Settings.hpp"

class EventManager
{
private:
	//event queue
	std::queue<sf::Event> events;
public:
	//add events to queue
	void pushEvent(const sf::Event& event);
	//returns if queue is empty or not
	bool hasEvents() const;
	//returns most recent event
	sf::Event popEvent();
	//clears the event queue
	void clear();
};