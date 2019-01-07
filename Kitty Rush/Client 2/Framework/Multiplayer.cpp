#include "player.h"
#include "Multiplayer.h"
#include "TextureHolder.h"

Multiplayer::Multiplayer()
{
	s_Speed = START_SPEED;
	s_Candy = START_ENERGY;
	s_MaxCandy = START_ENERGY;

	// Associate a texture with the sprite
	// !!Watch this space!!
	s_Sprite = Sprite(TextureHolder::GetTexture(
		"graphics/player1.png"));

	// Set the origin of the sprite to the centre, 
	// for smooth rotation
	s_Sprite.setOrigin(25, 25);
}

void Multiplayer::resetMultiplayerStats()
{
	s_Speed = START_SPEED;
	s_Candy = START_ENERGY;
	s_MaxCandy = START_ENERGY;
}

void Multiplayer::spawn(IntRect arena, Vector2f resolution, int tileSize)
{
	// Place the player in the middle of the arena
	s_Position.x = arena.width / 2.2;
	s_Position.y = arena.height / 2.2;

	// Copy the details of the arena to the player's s_Arena
	s_Arena.left = arena.left;
	s_Arena.width = arena.width;
	s_Arena.top = arena.top;
	s_Arena.height = arena.height;

	// Remember how big the tiles are in this arena
	s_TileSize = tileSize;

	// Strore the resolution for future use
	s_Resolution.x = resolution.x;
	s_Resolution.y = resolution.y;

}

Time Multiplayer::getLastHitTime()
{
	return s_LastHit;
}

bool Multiplayer::hit(Time timeHit)
{
	if (timeHit.asMilliseconds() - s_LastHit.asMilliseconds() > 200)// 2 tenths of second
	{
		s_LastHit = timeHit;
		s_Candy -= 10;
		return true;
	}
	else
	{
		return false;
	}

}

FloatRect Multiplayer::getPosition()
{
	return s_Sprite.getGlobalBounds();
}

Vector2f Multiplayer::getCenter()
{
	return s_Position;
}

float Multiplayer::getRotation()
{
	return s_Sprite.getRotation();
}

Sprite Multiplayer::getSprite()
{
	return s_Sprite;
}

int Multiplayer::getCandy()
{
	return s_Candy;
}

void Multiplayer::moveLeft()
{
	s_LeftPressed = true;
}

void Multiplayer::moveRight()
{
	s_RightPressed = true;
}

void Multiplayer::moveUp()
{
	s_UpPressed = true;
}

void Multiplayer::moveDown()
{
	s_DownPressed = true;
}

void Multiplayer::stopLeft()
{
	s_LeftPressed = false;
}

void Multiplayer::stopRight()
{
	s_RightPressed = false;
}

void Multiplayer::stopUp()
{
	s_UpPressed = false;
}

void Multiplayer::stopDown()
{
	s_DownPressed = false;
}

void Multiplayer::update(float elapsedTime, Vector2i mousePosition)
{

	if (s_UpPressed)
	{
		s_Position.y -= s_Speed * elapsedTime;
	}

	if (s_DownPressed)
	{
		s_Position.y += s_Speed * elapsedTime;
	}

	if (s_RightPressed)
	{
		s_Position.x += s_Speed * elapsedTime;
	}

	if (s_LeftPressed)
	{
		s_Position.x -= s_Speed * elapsedTime;
	}

	s_Sprite.setPosition(s_Position);



	/* Keep the player inside the arena, not neccessary
		 Commented so the character can move freely INSIDE THE ARENA, but not outside of it!

	if (s_Position.x > s_Arena.width - s_TileSize)
	{
		s_Position.x = s_Arena.width - s_TileSize;
	} */

	if (s_Position.x < s_Arena.left + s_TileSize)
	{
		s_Position.x = s_Arena.left + s_TileSize;
	}

	/*
		 Commented so the character can move freely INSIDE THE ARENA, but not outside of it!
	if (s_Position.y > s_Arena.height - s_TileSize)
	{
		s_Position.y = s_Arena.height - s_TileSize;
	}*/

	if (s_Position.y < s_Arena.top + s_TileSize)
	{
		s_Position.y = s_Arena.top + s_TileSize;
	}

	// Calculate the angle the player is facing
	float angle = (atan2(mousePosition.y - s_Resolution.y / 2,
		mousePosition.x - s_Resolution.x / 2)
		* 180) / 3.141;

	s_Sprite.setRotation(angle);
}

void Multiplayer::upgradeSpeed()
{
	// 20% speed upgrade
	s_Speed += (START_SPEED * .2);
}

void Multiplayer::upgradeCandy()
{
	// 20% max candy upgrade
	s_MaxCandy += (START_ENERGY * .2);

}

void Multiplayer::increaseCandyLevel(int amount)
{
	s_Candy += amount;

	// But not beyond the maximum
	if (s_Candy > s_MaxCandy)
	{
		s_Candy = s_MaxCandy;
	}
}

