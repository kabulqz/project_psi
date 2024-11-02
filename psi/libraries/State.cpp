#include "State.hpp"

void State::handleHoverState(Button& button, bool isCurrentlyHovered)
{
	//if the hover state has changed, restart the clock
	if(isCurrentlyHovered && !button.getHovered())
	{
		button.hoverClock.restart();
	}
	//update the button's hover state
	button.setHovered(isCurrentlyHovered);
}