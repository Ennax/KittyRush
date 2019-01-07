#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Fish
{
private:
	// Position
	Vector2f s_Position;

	// Shape of each fish
	RectangleShape s_FishShape;

	// Is this fish currently whizzing through the air
	bool s_InFlight = false;

	// How fast does a fish travel?
	float s_FishSpeed = 1000;

	// What fraction of 1 pixel does the fish travel, 
	// Horizontally and vertically each frame?
	// These values will be derived from s_FishSpeed
	float s_FishDistanceX;
	float s_FishDistanceY;

	// Where is this fish headed to?
	float s_XTarget;
	float s_YTarget;

	// Some boundaries so the fish doesn't fly forever
	float s_MaxX;
	float s_MinX;
	float s_MaxY;
	float s_MinY;

	// Public function prototypes go here
public:
	// The constructor
	Fish();

	// Stop the fish
	void stop();

	// Returns the value of s_InFlight
	bool isInFlight();

	// Launch a new fish
	void shoot(float startX, float startY,
		float xTarget, float yTarget);

	// Tell the calling code where the fish is in the world
	FloatRect getPosition();

	// Return the actual shape (for drawing)
	RectangleShape getShape();

	// Update the fish each frame
	void update(float elapsedTime);

};