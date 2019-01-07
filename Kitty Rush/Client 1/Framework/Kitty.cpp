#include "Kitty.h"
#include "TextureHolder.h"
#include <cstdlib>
#include <ctime>

using namespace std;


void Kitty::spawn(float startX, float startY, int type, int seed)
{

	switch (type)
	{
	case 0:
		// The fat kitty
		s_Sprite = Sprite(TextureHolder::GetTexture(
			"graphics/fatkitty.png"));

		s_Speed = 20;
		s_Candy = 3;
		break;

	case 1:
		// The fast kitty
		s_Sprite = Sprite(TextureHolder::GetTexture(
			"graphics/fastkitty.png"));

		s_Speed = 40;
		s_Candy = 1;
		break;

	case 2:
		// Lazy cat
		s_Sprite = Sprite(TextureHolder::GetTexture(
			"graphics/lazykitty.png"));

		s_Speed = 10;
		s_Candy = 2;
		break;
	}

	// You can modify the speed, after all, each type of 
	// kitty is different! This is a speed modifier.
	srand((int)time(0) * seed);
	// Somewhere between 80 an 100
	float modifier = (rand() % MAX_VARRIANCE) + OFFSET;
	// Express as a fraction of 1
	modifier /= 100; // Now equals between .7 and 1
	s_Speed *= modifier;

	s_Position.x = startX;
	s_Position.y = startY;

	s_Sprite.setOrigin(25, 25);
	s_Sprite.setPosition(s_Position);
}

bool Kitty::hit()
{
	s_Candy--;

	if (s_Candy < 0)
	{
		// if caught
		s_Alive = false;
		s_Sprite.setTexture(TextureHolder::GetTexture("graphics/caught.png"));

		return true;
	}

	// kitty left with almost no energy
	return false;
}

bool Kitty::isAlive()
{
	return s_Alive;
}

FloatRect Kitty::getPosition()
{
	return s_Sprite.getGlobalBounds();
}


Sprite Kitty::getSprite()
{
	return s_Sprite;
}

void Kitty::update(float elapsedTime,
	Vector2f playerLocation)
{
	float playerX = playerLocation.x;
	float playerY = playerLocation.y;

	// Update the Kitty position variables
	if (playerX > s_Position.x)
	{
		s_Position.x = s_Position.x +
			s_Speed * elapsedTime;
	}

	if (playerY > s_Position.y)
	{
		s_Position.y = s_Position.y +
			s_Speed * elapsedTime;
	}

	if (playerX < s_Position.x)
	{
		s_Position.x = s_Position.x -
			s_Speed * elapsedTime;
	}

	if (playerY < s_Position.y)
	{
		s_Position.y = s_Position.y -
			s_Speed * elapsedTime;
	}

	// Move the sprite
	s_Sprite.setPosition(s_Position);

	// Face the sprite in the correct direction
	float angle = (atan2(playerY - s_Position.y,
		playerX - s_Position.x)
		* 180) / 3.141;

	s_Sprite.setRotation(angle);


}