#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Player
{
private:
	const float START_SPEED = 150;
	const float START_ENERGY = 100;

	// position
	Vector2f s_Position;

	// sprite
	Sprite s_Sprite;

	// texture object
	Texture s_Texture;

	//Counterwalking so we can have a proper moving animation

	int counterWalking;

	//screen resolution
	Vector2f s_Resolution;

	// arena size
	IntRect s_Arena;

	// tile size
	int s_TileSize;

	// direction of the player
	bool s_UpPressed;
	bool s_DownPressed;
	bool s_LeftPressed;
	bool s_RightPressed;

	// How much Candy has the player got?
	int s_Candy;
	// What is the maximum Candy the player can have
	int s_MaxCandy;

	// When was the player last hit
	Time s_LastHit;

	// Speed in pixels per second
	float s_Speed;


	// All our public functions will come next
public:

	Player();

	// Call this at the end of every game
	void resetPlayerStats();

	void spawn(IntRect arena, Vector2f resolution, int tileSize);

	// Handle the player getting cuddled by a cat
	bool hit(Time timeHit);

	// How long ago was the player last cuddled
	Time getLastHitTime();

	// Where is the player
	FloatRect getPosition();

	// Where is the center of the player
	Vector2f getCenter();

	// Which angle is the player facing
	float getRotation();

	// Send a copy of the sprite to main
	Sprite getSprite();

	// How much Candy has the player currently got?
	int getCandy();

	// The next four functions move the player
	void moveLeft();

	void moveRight();

	void moveUp();

	void moveDown();

	// Stop the player moving in a specific direction
	void stopLeft();

	void stopRight();

	void stopUp();

	void stopDown();

	// We will call this function once every frame
	void update(float elapsedTime, Vector2i mousePosition);

	// Give player a speed boost
	void upgradeSpeed();

	// Give the player some Candy
	void upgradeCandy();

	// Increase the maximum amount of Candy the player can have
	void increaseCandyLevel(int amount);


};


