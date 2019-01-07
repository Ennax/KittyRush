#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Kitty
{
private:
	// How fast is each Kitty type?
	const float FATKITTY_SPEED = 30;
	const float FASTKITTY_SPEED = 60;
	const float LAZYKITTY_SPEED = 10;

	// How tough is each Kitty type
	const float FATKITTY_ENERGY = 3;
	const float FASTKITTY_ENERGY = 1;
	const float LAZYKITTY_ENERGY = 2;

	// Make each Kitty vary its speed slightly
	const int MAX_VARRIANCE = 30;
	const int OFFSET = 101 - MAX_VARRIANCE;

	// Where is this Kitty?
	Vector2f s_Position;

	// A sprite for the Kitty
	Sprite s_Sprite;

	// How fast can this one run/crawl?
	float s_Speed;

	// How much ENERGY has it got?
	float s_Candy;

	// Is it still alive?
	bool s_Alive;

	// Public prototypes go here	
public:

	// Handle when a fish hits a Kitty
	bool hit();

	// Find out if the Kitty is alive
	bool isAlive();

	// Spawn a new Kitty
	void spawn(float startX, float startY, int type, int seed);

	// Return a rectangle that is the position in the world
	FloatRect getPosition();

	// Get a copy of the sprite to draw
	Sprite getSprite();

	// Update the Kitty each frame
	void update(float elapsedTime, Vector2f playerLocation);
};


