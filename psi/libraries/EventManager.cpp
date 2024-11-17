#include "EventManager.hpp"

//add events to queue
void EventManager::pushEvent(const sf::Event& event)
{
	events.push(event);
}

//returns if queue is empty or not
bool EventManager::hasEvents() const
{
	return !events.empty();
}

//returns most recent event
sf::Event EventManager::popEvent()
{
	if (events.empty())
	{
		return {};
	}

	sf::Event event = events.front();
	events.pop();
	return event;
}

//clears the event queue
void EventManager::clear()
{
	while (!events.empty())
	{
		events.pop();
	}
}