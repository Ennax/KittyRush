#include "player.h"
#include "TextureHolder.h"
#include "Lua_Edit_Manager.h"
Player::Player()
{
	s_Speed = START_SPEED;
	s_Candy = START_ENERGY;
	s_MaxCandy = START_ENERGY;
	//For the animation:
	counterWalking = 0;

	// Associate a texture with the sprite
	// !!Watch this space!!

	Lua_Edit_Manager luaData;
	tuple<string> texture_Data = luaData.texture_Data();

	string luaTexture = (get<0>(texture_Data));
	s_Sprite = Sprite(TextureHolder::GetTexture(luaTexture));

	// Set the origin of the sprite to the centre, 
	// for smooth rotation
	s_Sprite.setOrigin(25, 25);

	s_Sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
}

void Player::resetPlayerStats()
{
	s_Speed = START_SPEED;
	s_Candy = START_ENERGY;
	s_MaxCandy = START_ENERGY;
}

void Player::spawn(IntRect arena, Vector2f resolution, int tileSize)
{
	// Place the player in the middle of the arena
	s_Position.x = arena.width / 2;
	s_Position.y = arena.height / 2;

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

Time Player::getLastHitTime()
{
	return s_LastHit;
}

bool Player::hit(Time timeHit)
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

FloatRect Player::getPosition()
{
	return s_Sprite.getGlobalBounds();
}

Vector2f Player::getCenter()
{
	return s_Position;
}

float Player::getRotation()
{
	return s_Sprite.getRotation();
}

Sprite Player::getSprite()
{
	return s_Sprite;
}

int Player::getCandy()
{
	return s_Candy;
}

void Player::moveLeft()
{
	s_LeftPressed = true;
	//moving left
	s_Sprite.setTextureRect(sf::IntRect(counterWalking * 32, 32 * 1, 32, 32));
	counterWalking++;
	if (counterWalking == 2)

		counterWalking = 0;
}

void Player::moveRight()
{
	s_RightPressed = true;
	//moving right
	s_Sprite.setTextureRect(sf::IntRect(counterWalking * 32, 32 * 2, 32, 32));
	counterWalking++;
	if (counterWalking == 2)

		counterWalking = 0;
}

void Player::moveUp()
{
	s_UpPressed = true;
	//moving Up
	s_Sprite.setTextureRect(sf::IntRect(counterWalking*32, 32*3, 32, 32));
	counterWalking++;
	if (counterWalking == 2)

		counterWalking = 0;
}

void Player::moveDown()
{
	s_DownPressed = true;
	//moving down
	s_Sprite.setTextureRect(sf::IntRect(counterWalking * 32, 0, 32, 32));
	counterWalking++;
	if (counterWalking == 2)

		counterWalking = 0;

}


void Player::stopLeft()
{
	s_LeftPressed = false;
}

void Player::stopRight()
{
	s_RightPressed = false;
}

void Player::stopUp()
{
	s_UpPressed = false;
}

void Player::stopDown()
{
	s_DownPressed = false;
}

void Player::update(float elapsedTime, Vector2i mousePosition)
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



	// Keep the player inside the arena, not neccessary
		// Commented so the character can move freely INSIDE THE ARENA, but not outside of it!

	/*if (s_Position.x > s_Arena.width - s_TileSize)
	{
		s_Position.x = s_Arena.width - s_TileSize;
	} */

	if (s_Position.x < s_Arena.left + s_TileSize)
	{
		s_Position.x = s_Arena.left + s_TileSize;
	}

	
		 //Up and down
	if (s_Position.y > s_Arena.height - s_TileSize)
	{
		s_Position.y = s_Arena.height - s_TileSize;
	}

	if (s_Position.y < s_Arena.top + s_TileSize)
	{
		s_Position.y = s_Arena.top + s_TileSize;
	}

// Uncomment if you want the sprite to look at the mouse position:

/*	float angle = (atan2(mousePosition.y - s_Resolution.y / 2,
		mousePosition.x - s_Resolution.x / 2)
		* 180) / 3.141;

	s_Sprite.setRotation(angle); */
}

void Player::upgradeSpeed()
{
	// 20% speed upgrade
	s_Speed += (START_SPEED * .2);
}

void Player::upgradeCandy()
{
	// 20% max candy upgrade
	s_MaxCandy += (START_ENERGY * .2);

}

void Player::increaseCandyLevel(int amount)
{
	s_Candy += amount;

	// But not beyond the maximum
	if (s_Candy > s_MaxCandy)
	{
		s_Candy = s_MaxCandy;
	}
}

