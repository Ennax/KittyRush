
#include "fish.h"
#include "TextureHolder.h"


// The constructor
Fish::Fish()
{
	s_FishShape.setSize(sf::Vector2f(2, 2));
}

void Fish::shoot(float startX, float startY,
	float targetX, float targetY)
{
	// Keep track of the fish
	s_InFlight = true;
	s_Position.x = startX;
	s_Position.y = startY;

	// Calculate the gradient of the flight path
	float gradient = (startX - targetX) / (startY - targetY);

	// Any gradient less than zero needs to be negative
	if (gradient < 0)
	{
		gradient *= -1;
	}

	// Calculate the ratio between x and t
	float ratioXY = s_FishSpeed / (1 + gradient);

	// Set the "speed" horizontally and vertically
	s_FishDistanceY = ratioXY;
	s_FishDistanceX = ratioXY * gradient;

	// Point the fish in the right direction
	if (targetX < startX)
	{
		s_FishDistanceX *= -1;
	}

	if (targetY < startY)
	{
		s_FishDistanceY *= -1;
	}

	// Finally, assign the results to the
	// member variables
	s_XTarget = targetX;
	s_YTarget = targetY;

	// Set a max range of 1000 pixels
	float range = 1000;
	s_MinX = startX - range;
	s_MaxX = startX + range;
	s_MinY = startY - range;
	s_MaxY = startY + range;

	// Position the fish ready to be drawn
	s_FishShape.setPosition(s_Position);
}

void Fish::stop()
{
	s_InFlight = false;
}

bool Fish::isInFlight()
{
	return s_InFlight;
}

FloatRect Fish::getPosition()
{
	return s_FishShape.getGlobalBounds();
}


RectangleShape Fish::getShape()
{
	return s_FishShape;
}


void Fish::update(float elapsedTime)
{
	// Update the fish position variables
	s_Position.x += s_FishDistanceX * elapsedTime;
	s_Position.y += s_FishDistanceY * elapsedTime;

	// Move the fish
	s_FishShape.setPosition(s_Position);

	// Has the fish gone out of range?
	if (s_Position.x < s_MinX || s_Position.x > s_MaxX ||
		s_Position.y < s_MinY || s_Position.y > s_MaxY)
	{
		s_InFlight = false;
	}

}