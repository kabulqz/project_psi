#include "EventManager.hpp"

void EventManager::pushEvent(const sf::Event& event)
{
	events.push(event);
}

bool EventManager::hasEvents() const
{
	return !events.empty();
}

sf::Event EventManager::popEvent()
{
	if (events.empty())
	{
		return sf::Event();
	}

	sf::Event event = events.front();
	events.pop();
	return event;
}

void EventManager::clear()
{
	while (!events.empty())
	{
		events.pop();
	}
}