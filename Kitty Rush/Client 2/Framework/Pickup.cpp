#include "Pickup.h"
#include "TextureHolder.h"

Pickup::Pickup(int type)
{
	// Store the type of this pickup
	s_Type = type;

	// Associate the texture with the sprite
	if (s_Type == 1)
	{
		s_Sprite = Sprite(TextureHolder::GetTexture(
			"graphics/candy_pickup.png"));

		// How much is pickup worth
		s_Value = CANDY_START_VALUE;

	}
	else
	{
		s_Sprite = Sprite(TextureHolder::GetTexture(
			"graphics/fish_pickup.png"));

		// How much is pickup worth
		s_Value = FISH_START_VALUE;
	}

	s_Sprite.setOrigin(25, 25);

	s_SecondsToLive = START_SECONDS_TO_LIVE;
	s_SecondsToWait = START_WAIT_TIME;
}

void Pickup::setArena(IntRect arena)
{

	// Copy the details of the arena to the pickup's s_Arena
	s_Arena.left = arena.left + 50;
	s_Arena.width = arena.width - 50;
	s_Arena.top = arena.top + 50;
	s_Arena.height = arena.height - 50;

	spawn();
}

void Pickup::spawn()
{
	// Spawn at a random location
	srand((int)time(0) / s_Type);
	int x = (rand() % s_Arena.width);
	srand((int)time(0) * s_Type);
	int y = (rand() % s_Arena.height);

	// Not currently spawned
	//s_Spawned = false;
	s_SecondsSinceSpawn = 0;
	s_Spawned = true;

	s_Sprite.setPosition(x, y);
}

FloatRect Pickup::getPosition()
{
	return s_Sprite.getGlobalBounds();
}

Sprite Pickup::getSprite()
{
	return s_Sprite;
}

bool Pickup::isSpawned()
{
	return s_Spawned;
}

int Pickup::gotIt()
{
	s_Spawned = false;
	s_SecondsSinceDeSpawn = 0;
	return s_Value;
}

void Pickup::update(float elapsedTime)
{
	if (s_Spawned)
	{
		s_SecondsSinceSpawn += elapsedTime;
	}
	else
	{
		s_SecondsSinceDeSpawn += elapsedTime;
	}


	// Do we need to hide a pickup?
	if (s_SecondsSinceSpawn > s_SecondsToLive && s_Spawned)
	{
		// Revove the pickup and put it somewhere else
		s_Spawned = false;
		s_SecondsSinceDeSpawn = 0;
	}

	// Do we need to spawn a pickup
	if (s_SecondsSinceDeSpawn > s_SecondsToWait && !s_Spawned)
	{
		// spawn the pickup and reset the timer
		spawn();
	}

}

void Pickup::upgrade()
{
	if (s_Type == 1)
	{
		s_Value += (CANDY_START_VALUE * .5);
	}
	else
	{
		s_Value += (FISH_START_VALUE * .5);
	}

	// Make them more frequent and last longer
	s_SecondsToLive += (START_SECONDS_TO_LIVE / 10);
	s_SecondsToWait -= (START_WAIT_TIME / 10);
}
