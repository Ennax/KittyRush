#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "KittyArena.h"
#include "Player.h"
#include "TextureHolder.h"
#include "Fish.h"
#include "Pickup.h"
#include "Kitty.h"
#include "gameOver.h"

//  ENET LIBRARY
// Iostream so that EnET can output the packets (you can buypass this, but change the code)
#include <iostream>
#include <enet\enet.h>
using namespace std; // for cout<<endl;
//
// Lua 
#include "Lua_Edit_Manager.h"

using namespace sf;

// ENET VECTOR FOR POSSITION !
struct Vector2ENET {
	float x;
	float y;
};

struct Vector2Enemy {
	float x;
	float y;
};
int main()
{
	// Here is the instabce of TextureHolder
	TextureHolder holder;

	// The game will always be in one of four states
	enum class State { PAUSED, LEVELING_UP, GAME_OVER, PLAYING };
	// Start with the GAME_OVER state
	State state = State::GAME_OVER;

	
	// Get the screen resolution and create an SFML window
	
	Vector2f resolution;
	//resolution.x = VideoMode::getDesktopMode().width;
	//resolution.y = VideoMode::getDesktopMode().height;
	
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;
	gameOver menu(resolution.x, resolution.y);

	/// LUA
	Lua_Edit_Manager luaData;
	tuple<string, int, int> window_Data = luaData.window_Data();

	
	//RenderWindow window(VideoMode(resolution.x, resolution.y),
	//	"Client Window", Style::Fullscreen);
	
	RenderWindow window(VideoMode(get<1>(window_Data), get<2>(window_Data)), get<0>(window_Data));

	// Lua Extension


	//RenderWindow window(VideoMode(resolution.x, resolution.y),
	//"Client Window");

	//
	// ENET SETUP BEGIN 
	//

	// We create an instance for the PLAYER 2! 

	Vector2Enemy position;
	position.x = 600.0f;
	position.y = 100.0f;

	sf::RectangleShape enemy;

	enemy.setSize(sf::Vector2f(100.0f, 100.0f));

	enemy.setPosition(sf::Vector2f(position.x, position.y));

	sf::Texture testTexture;

	if (!testTexture.loadFromFile("graphics/player1.png"))
	{
		cout << "Texture did not load!" << "\n";
	}
	enemy.setTexture(&testTexture);
	// We create an instance for the PLAYER 2!
		/* Now we're declaring our Vector2 - this vector will be controlling the
	position of an 'enemy' entity in the Client instance. */

	

	/* We initialise the ENet library, and double-check this has worked. We then
	declare a few library specific variables. An address (which represents the
	server address), a Host (which represents the server, i.e. this software
	instance), and an arbitrary Event. ENet is an event-driven library, and functions
	similarly to SFML in that regard. Just like we poll keyboard events from SFML's
	Event type, we can do the same for ENet's. */

	if (enet_initialize() != 0)
	{
		cout << "Enet failed to initialise!" << "\n\n";
	}

	ENetAddress address;
	ENetHost* server;
	ENetEvent enetEvent;

	ENetPacket* dataPacket;

	/* We set the address values to essentially be a local host */

	address.host = ENET_HOST_ANY;
	address.port = 1234;


	server = enet_host_create(&address, 32, 2, 0, 0);



	if (server == NULL)
	{
		cout << "Server failed to initialise!" << "\n\n";
	}

	//
	// ENET SETUP END // ENET SETUP END 
	//


	// Create a an SFML View for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Here is our clock for timing everything
	Clock clock;
	// How long has the PLAYING state been active
	Time gameTimeTotal;

	// Where is the mouse in relation to world coordinates
	Vector2f mouseWorldPosition;
	// Where is the mouse in relation to screen coordinates
	Vector2i mouseScreenPosition;

	// Create an instance of the Player class
	Player player;

	// The boundaries of the arena
	IntRect arena;

	// Create the background
	VertexArray background;

	
	Lua_Edit_Manager luaData2;
	tuple<string> texture_Background = luaData2.texture_Background();


	//RenderWindow window(VideoMode(resolution.x, resolution.y),
	//	"Client Window", Style::Fullscreen);

	string lua_background = (get<0>(texture_Background));


	// Load the texture for our background vertex array
	Texture textureBackground = TextureHolder::GetTexture(lua_background);

	// Prepare for a bunch of kitties
	int numKitties;
	int numKittiesAlive;
	Kitty* kitties = NULL;

	// 200 cat foodies
	Fish fishs[200];
	int currentFish = 50;
	int fishsSpare = 70;
	int fishsInClip = 10;
	int clipSize = 10;
	float fishRate = 1;
	// When was the fire button last pressed?
	Time lastPressed;

	// Hide the mouse pointer and replace it with crosshair
	window.setMouseCursorVisible(true);
	Sprite spriteCrosshair;

	Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	// Create a couple of pickups
	Pickup candyPickup(1);
	Pickup fishPickup(4);

	// About the game
	int score = 0;
	int hiScore = 0;

	// For the home/game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/startmenu.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	// For the home/game over screen
	Sprite spriteChoiceOption;
	Texture textureChoiceOption = TextureHolder::GetTexture("graphics/background.png");
	spriteChoiceOption.setTexture(textureChoiceOption);
	spriteChoiceOption.setPosition(0, 0);

	// Create a view for the HUD
	View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Create a sprite for the fish icon
	Sprite spriteFishIcon;
	Texture textureFishIcon = TextureHolder::GetTexture("graphics/fish_icon.png");
	spriteFishIcon.setTexture(textureFishIcon);
	spriteFishIcon.setPosition(20, 980);

	// Load the font
	Font font;
	Font font1;
	Font font2;

	font.loadFromFile("fonts/HelloCute.ttf");
	font1.loadFromFile("fonts/Bubblegum.ttf");
	font2.loadFromFile("fonts/VCR_OSD_MONO_1.001.ttf");

	// Paused

		// Create a sprite for the Paused menu icon
	Sprite spritePausedIcon;
	Texture texturePausedIcon = TextureHolder::GetTexture("graphics/pausedbutton.png");
	spritePausedIcon.setTexture(texturePausedIcon);
	// ( formula to place it in mid of the screen)
	spritePausedIcon.setPosition((resolution.x / 2) - (spritePausedIcon.getGlobalBounds().width / 2), (resolution.y / 2) - spritePausedIcon.getGlobalBounds().height / 4);

	// create texst for the paused menu
	Text pausedText;
	Text pausedText1;
	pausedText.setFont(font1);
	pausedText.setCharacterSize(70);
	pausedText.setFillColor(Color::White);
	pausedText.setPosition(525, (resolution.y / 1.5) - (pausedText.getGlobalBounds().height / 3));
	pausedText.setString("To continue press ENTER");

	pausedText1.setFont(font1);
	pausedText1.setCharacterSize(70);
	pausedText1.setFillColor(Color::White);
	pausedText1.setPosition((resolution.x / 3), (resolution.y / 1.3) - (pausedText1.getGlobalBounds().height / 3));
	pausedText1.setString("To quit press ESC");

	// Create a sprite for the menu icon: NAME OF THE GAME BUTTON
	Sprite spriteKittyRushIcon;
	Texture textureKittyRushIcon = TextureHolder::GetTexture("graphics/kittytitle.png");
	spriteKittyRushIcon.setTexture(textureKittyRushIcon);
	// ( formula to place it in mid of the screen)
	spriteKittyRushIcon.setPosition((resolution.x / 2) - (spriteKittyRushIcon.getGlobalBounds().width / 2), spriteKittyRushIcon.getGlobalBounds().height / 4);

	// Create a sprite for the menu icon: PLAY BUTTON
	Sprite spritePlayIcon;
	Texture texturePlayIcon = TextureHolder::GetTexture("graphics/playbutton2.png");
	spritePlayIcon.setTexture(texturePlayIcon);
	// ( formula to place it in mid of the screen)
	spritePlayIcon.setPosition((resolution.x / 2) - (spritePlayIcon.getGlobalBounds().width / 2), (resolution.y / 2) - (spritePlayIcon.getGlobalBounds().height / 3));

	//Create a sprite for the menu icon: QUIT BUTTON
	Sprite spriteQuitIcon;
	Texture textureQuitIcon = TextureHolder::GetTexture("graphics/quitbutton2.png");
	spriteQuitIcon.setTexture(textureQuitIcon);
	// ( formula to place it in mid of the screen)
	spriteQuitIcon.setPosition((resolution.x / 2) - (spriteQuitIcon.getGlobalBounds().width / 2), (resolution.y / 1.3) - (spriteQuitIcon.getGlobalBounds().height / 3));




	// Game Over
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(100);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(1050, 540);
	gameOverText.setString("Press Enter to play");

	// Create a sprite for the Levels icon
	Sprite spritechooseLevelIcon;
	Texture texturechooseLevelIcon = TextureHolder::GetTexture("graphics/choose.png");
	spritechooseLevelIcon.setTexture(texturechooseLevelIcon);
	// ( formula to place it in mid of the screen)
	spritechooseLevelIcon.setPosition((resolution.x / 2) - (spritechooseLevelIcon.getGlobalBounds().width / 2), spritechooseLevelIcon.getGlobalBounds().height / 4);


	//Create a sprite for the LEVEL 1 icon:
	Sprite spritelevel1Icon;
	Texture texturelevel1Icon = TextureHolder::GetTexture("graphics/1.png");
	spritelevel1Icon.setTexture(texturelevel1Icon);
	// ( formula to place it in mid of the screen)
	spritelevel1Icon.setPosition((resolution.x / 2) - (spritelevel1Icon.getGlobalBounds().width / 2), (resolution.y / 2.2) - (spritelevel1Icon.getGlobalBounds().height / 3));

	//Create a sprite for the LEVEL 2 icon:
	Sprite spritelevel2Icon;
	Texture texturelevel2Icon = TextureHolder::GetTexture("graphics/2.png");
	spritelevel2Icon.setTexture(texturelevel2Icon);
	// ( formula to place it in mid of the screen)
	spritelevel2Icon.setPosition((resolution.x / 2) - (spritelevel2Icon.getGlobalBounds().width / 2), (resolution.y / 1.9) - (spritelevel2Icon.getGlobalBounds().height / 3));

	//Create a sprite for the LEVEL 3 icon:
	Sprite spritelevel3Icon;
	Texture texturelevel3Icon = TextureHolder::GetTexture("graphics/3.png");
	spritelevel3Icon.setTexture(texturelevel3Icon);
	// ( formula to place it in mid of the screen)
	spritelevel3Icon.setPosition((resolution.x / 2) - (spritelevel3Icon.getGlobalBounds().width / 2), (resolution.y / 1.69) - (spritelevel3Icon.getGlobalBounds().height / 3));

	//Create a sprite for the LEVEL 4 icon:
	Sprite spritelevel4Icon;
	Texture texturelevel4Icon = TextureHolder::GetTexture("graphics/4.png");
	spritelevel4Icon.setTexture(texturelevel4Icon);
	// ( formula to place it in mid of the screen)
	spritelevel4Icon.setPosition((resolution.x / 2) - (spritelevel4Icon.getGlobalBounds().width / 2), (resolution.y / 1.5) - (spritelevel4Icon.getGlobalBounds().height / 3));

	//Create a sprite for the LEVEL 5 icon:
	Sprite spritelevel5Icon;
	Texture texturelevel5Icon = TextureHolder::GetTexture("graphics/5.png");
	spritelevel5Icon.setTexture(texturelevel5Icon);
	// ( formula to place it in mid of the screen)
	spritelevel5Icon.setPosition((resolution.x / 2) - (spritelevel5Icon.getGlobalBounds().width / 2), (resolution.y / 1.36) - (spritelevel5Icon.getGlobalBounds().height / 3));


	// Fish
	Text fishText;
	fishText.setFont(font1);
	fishText.setCharacterSize(55);
	fishText.setFillColor(Color::White);
	fishText.setPosition(200, 980);

	// Score
	Text scoreText;
	scoreText.setFont(font1);
	scoreText.setCharacterSize(55);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(1600, 0);

	// Load the high score from a text file/
	std::ifstream inputFile("Score/score.txt");
	if (inputFile.is_open())
	{
		inputFile >> hiScore;
		inputFile.close();
	}

	// Hi Score
	Text hiScoreText;
	hiScoreText.setFont(font1);
	hiScoreText.setCharacterSize(55);
	hiScoreText.setFillColor(Color::White);
	hiScoreText.setPosition(80, 0);
	std::stringstream s;
	s << "High Score:" << hiScore;
	hiScoreText.setString(s.str());

	// Kitties remaining
	Text kittyRemainingText;
	kittyRemainingText.setFont(font);
	kittyRemainingText.setCharacterSize(55);
	kittyRemainingText.setFillColor(Color::White);
	kittyRemainingText.setPosition(1500, 980);
	kittyRemainingText.setString("Kitties: 100");

	// Wave number
	int wave = 0;
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(55);
	waveNumberText.setFillColor(Color::White);
	waveNumberText.setPosition(1250, 980);
	waveNumberText.setString("Wave: 0");

	// candy bar
	// Changed the color in RGB
	sf::Color color(34, 139, 34);
	RectangleShape candyBar;
	candyBar.setFillColor(color);
	candyBar.setPosition(450, 980);

	// When did we last update the HUD?
	int framesSinceLastHUDUpdate = 0;
	// What time was the last update
	Time timeSinceLastUpdate;
	// How often is the HUD updating (score, highscore, cats, and so on)
	int fpsMeasurementFrameInterval = 20;

	/*background music 
	SoundBuffer backgroundmusicBuffer;
	backgroundmusicBuffer.loadFromFile("sound/background.wav");
	Sound  backgroundmusic;
	backgroundmusic.setBuffer(backgroundmusicBuffer); */

	sf::Music music;

	// Prepare the hit sound
	SoundBuffer hitBuffer;
	hitBuffer.loadFromFile("sound/hit.wav");
	Sound hit;
	hit.setBuffer(hitBuffer);

	// Prepare the splat sound
	SoundBuffer splatBuffer;
	splatBuffer.loadFromFile("sound/meowv2.wav");
	sf::Sound splat;
	splat.setBuffer(splatBuffer);

	// Prepare the shoot sound
	SoundBuffer shootBuffer;
	shootBuffer.loadFromFile("sound/socute.wav");
	Sound shoot;
	shoot.setBuffer(shootBuffer);

	// Prepare the reload sound
	SoundBuffer reloadBuffer;
	reloadBuffer.loadFromFile("sound/reload.wav");
	Sound reload;
	reload.setBuffer(reloadBuffer);

	// Prepare the failed sound
	SoundBuffer reloadFailedBuffer;
	reloadFailedBuffer.loadFromFile("sound/no_left_fish.wav");
	Sound reloadFailed;
	reloadFailed.setBuffer(reloadFailedBuffer);

	// Prepare the powerup sound
	SoundBuffer powerupBuffer;
	powerupBuffer.loadFromFile("sound/powerup.wav");
	Sound powerup;
	powerup.setBuffer(powerupBuffer);

	// Prepare the pickup sound
	SoundBuffer pickupBuffer;
	pickupBuffer.loadFromFile("sound/pickup.wav");
	Sound pickup;
	pickup.setBuffer(pickupBuffer);

	
	// The main game loop
	while (window.isOpen())
	{
		/*
		************
		Handle input
		************
		*/

		// Handle events
		Event event;

	
		while (window.pollEvent(event))
		{

			// ENET
			while (enet_host_service(server, &enetEvent, 0) > 0)
			{
				switch (enetEvent.type)
				{
				case ENET_EVENT_TYPE_CONNECT:

					cout << "A client connected from address " << enetEvent.peer->address.host << ":" << enetEvent.peer->address.port << ".\n";

					/* This event type has an associated peer: the client which has
					connected to the server. We can store some data about this peer
					for as long as it remains in scope using the "data" variable. */

					enetEvent.peer->data = "This is a client";

					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					cout << "The client from address " << enetEvent.peer->address.host << ":" << enetEvent.peer->address.port << " disconnected \n";

					enetEvent.peer->data = NULL;

					break;
				}
			}
			// ENET
			//enet
			if (event.type == Event::KeyPressed)
			{
			
				// Pause a game while playing
				if (event.key.code == Keyboard::P &&
					state == State::PLAYING)
				{
					state = State::PAUSED;

					if (event.key.code == Keyboard::Escape)
					{
						// ENET
						//if (peer != NULL)
						//{
						//	enet_peer_disconnect_now(peer, 0);
						//}
						// ENET
						window.close();
					}
				}

				// Restart while paused
				else if (event.key.code == Keyboard::Return &&
					state == State::PAUSED)
				{
					state = State::PLAYING;
					
					// Reset the clock so there isn't a frame jump
					clock.restart();
				}

				// Start a new game while in GAME_OVER state
				else if (event.key.code == Keyboard::Return &&
					state == State::GAME_OVER)
				{
					state = State::LEVELING_UP;
					wave = 0;
					score = 0;

					// Prepare the gun and fish for next game
					currentFish = 0;
					fishsSpare = 24;
					fishsInClip = 6;
					clipSize = 6;
					fishRate = 1;

					// Reset the player's stats
					player.resetPlayerStats();
				}
				// The arena can be zoomed in and rotated

				//if (Keyboard::isKeyPressed(Keyboard::Left))
				//{
					//mainView.rotate(.5f);
				//}

				//if (Keyboard::isKeyPressed(Keyboard::Right))
				//{
					//mainView.rotate(-.5f);
				//}
				// Handle the pressing and releasing of the WASD keys

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
				{
					position.x += -1.6f;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
				{
					position.x += 1.6f;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
				{
					position.y += -1.6f;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
				{
					position.y += 1.6f;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					window.close();
				}
				enemy.setPosition(sf::Vector2f(position.x, position.y));
				cout << "The position of the entity is (" << position.x << "," << position.y << ")\n";

				/* We populate the packet we made earlier using the enet_packet_create function, which accepts
				a reference to our Vector 2, the size of the data being sent (so, one Vector2 in this case), and
				flags whether or not the packet receipt needs verifying (in this example, we say that it does,
				but in reality it doesn't necessarily need to). The host then broadcasts the data packet to all
				connected clients across Channel 0. */

				dataPacket = enet_packet_create(&position, sizeof(Vector2Enemy), ENET_PACKET_FLAG_RELIABLE);
				enet_host_broadcast(server, 0, dataPacket);
				if (Keyboard::isKeyPressed(Keyboard::Up))
				{
					mainView.zoom(.99f);
				}

				if (Keyboard::isKeyPressed(Keyboard::Down))
				{
					mainView.zoom(1.01f);
				}
				// End spinning and zooming

				if (state == State::PLAYING)
				{	
					
					// Reloading
					if (event.key.code == Keyboard::R)
					{
						if (fishsSpare >= clipSize)
						{
							// Plenty of fishs. Reload.
							fishsInClip = clipSize;
							fishsSpare -= clipSize;
							reload.play();
						}
						else if (fishsSpare > 0)
						{
							// Only few fishs left
							fishsInClip = fishsSpare;
							fishsSpare = 0;
							reload.play();
						}
						else
						{
							// More here soon?!
							reloadFailed.play();
						}
					}
				}

			}
		}// End event polling

		 // Handle the player quitting
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			// ENET
		//	if (peer != NULL)
		//	{
		//		enet_peer_disconnect_now(peer, 0);
		//	}
			// ENET
			window.close();
		}

		// Handle controls while playing
		if (state == State::PLAYING)
		{
			
			// Fire a fish
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{

				if (gameTimeTotal.asMilliseconds()
					- lastPressed.asMilliseconds()
					> 1000 / fishRate && fishsInClip > 0)
				{

					// Pass the centre of the player and the centre of the crosshair
					// to the shoot function
					fishs[currentFish].shoot(
						player.getCenter().x, player.getCenter().y,
						mouseWorldPosition.x, mouseWorldPosition.y);

					currentFish++;
					if (currentFish > 99)
					{
						currentFish = 0;
					}
					lastPressed = gameTimeTotal;
					shoot.play();
					fishsInClip--;
				}

			}// End fire a fish

		}// End WASD while playing

		// Handle the levelling up state
		if (state == State::LEVELING_UP)
		{
			// Handle the player levelling up
			if (event.key.code == Keyboard::Num1)
			{
				// Increase the fish rate
				fishRate++;
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num2)
			{
				// More kitties to cuddle
				wave++;
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num3)
			{
				// Increase candy
				player.upgradeCandy();
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num4)
			{
				// Increase speed
				player.upgradeSpeed();
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num5)
			{
				candyPickup.upgrade();
				fishPickup.upgrade();
				wave++;
				state = State::PLAYING;
			}

			if (event.key.code == Keyboard::Num6)
			{

				state = State::PLAYING;
			}

			if (state == State::PLAYING)
			{
				// Increase the wave number
				wave++;

				// Level preparation
				arena.width = 1000 * wave;
				arena.height = 1000 * wave;
				arena.left = 0;
				arena.top = 0;

				// Pass the vertex array by reference 
				// to the createBackground function
				int tileSize = createBackground(background, arena);
				//int tileSize1 = createOverLay(background, arena);


				// Spawn the player in the middle of the arena
				player.spawn(arena, resolution, tileSize);

				// Configure the pick-ups
				candyPickup.setArena(arena);
				fishPickup.setArena(arena);

				// Create a bunch of lovely kitties
				numKitties = 12 * wave;

				// Delete the previously allocated memory (if it exists)
				delete[] kitties;
				kitties = createHorde(numKitties, arena);
				numKittiesAlive = numKitties;

				// Play the powerup sound
				powerup.play();

				// Reset the clock so there isn't a frame jump
				clock.restart();
			}
		}// End levelling up

		 /*
		 ****************
		 UPDATE THE FRAME
		 ****************
		 */
		if (state == State::PLAYING)
		{
			// Update the delta time
			Time dt = clock.restart();
			// Update the total game time
			gameTimeTotal += dt;
			// Make a decimal fraction of 1 from the delta time
			float dtAsSeconds = dt.asSeconds();

			// Where is the mouse pointer
			mouseScreenPosition = Mouse::getPosition();

			// Convert mouse position to world coordinates of mainView
			mouseWorldPosition = window.mapPixelToCoords(
				Mouse::getPosition(), mainView);

			// Set the crosshair to the mouse world location
			spriteCrosshair.setPosition(mouseWorldPosition);

			// Update the player
			player.update(dtAsSeconds, Mouse::getPosition());

			// Make a note of the players new position
			Vector2f playerPosition(player.getCenter());

			// Make the view centre around the player				
			mainView.setCenter(player.getCenter());

			// Loop through each type of kitty and update them constantly
			for (int i = 0; i < numKitties; i++)
			{
				if (kitties[i].isAlive())
				{
					kitties[i].update(dt.asSeconds(), playerPosition);
				}
			}

			// Update any fishs that are in-flight
			for (int i = 0; i < 100; i++)
			{
				if (fishs[i].isInFlight())
				{
					fishs[i].update(dtAsSeconds);
				}
			}

			// Update the pickups
			candyPickup.update(dtAsSeconds);
			fishPickup.update(dtAsSeconds);

			// Collision detection
			// Did you catch any kitties?
			for (int i = 0; i < 100; i++)
			{
				for (int j = 0; j < numKitties; j++)
				{
					if (fishs[i].isInFlight() &&
						kitties[j].isAlive())
					{
						if (fishs[i].getPosition().intersects
						(kitties[j].getPosition()))
						{
							// Stop the fish
							fishs[i].stop();

							// Register the hit and see if it was a catch
							if (kitties[j].hit()) {
								// Not just a fish to the cat but a catch too
								score += 10;
								if (score >= hiScore)
								{
									hiScore = score;
								}

								numKittiesAlive--;

								// When all the cats are caught (again)
								if (numKittiesAlive == 0) {
									state = State::LEVELING_UP;
								}
							}

							// Make a splat sound
							splat.play();

						}
					}

				}
			}// End kitty being caught

			// Have any kitties touched the player			
			for (int i = 0; i < numKitties; i++)
			{
				if (player.getPosition().intersects
				(kitties[i].getPosition()) && kitties[i].isAlive())
				{

					if (player.hit(gameTimeTotal))
					{
						// More here later
						hit.play();
					}

					if (player.getCandy() <= 0)
					{

						state = State::GAME_OVER;
						std::ofstream outputFile("Score/score.txt");

						outputFile << hiScore;
						outputFile.close();

					}
				}
			}// End player touched

			// Has the player touched candy pickup
			if (player.getPosition().intersects
			(candyPickup.getPosition()) && candyPickup.isSpawned())
			{
				player.increaseCandyLevel(candyPickup.gotIt());
				// Play a sound
				pickup.play();

			}

			// Has the player touched fish pickup
			if (player.getPosition().intersects
			(fishPickup.getPosition()) && fishPickup.isSpawned())
			{
				fishsSpare += fishPickup.gotIt();
				// Play a sound
				reload.play();

			}

			// size up the candy bar
			candyBar.setSize(Vector2f(player.getCandy() * 3, 70));

			// Increment the amount of time since the last HUD update
			timeSinceLastUpdate += dt;
			// Increment the number of frames since the last HUD calculation
			framesSinceLastHUDUpdate++;
			// Calculate FPS every fpsMeasurementFrameInterval frames
			if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval)
			{

				// Update game HUD text
				std::stringstream ssFish;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssWave;
				std::stringstream ssKittiesAlive;

				// Update the fish text
				ssFish << fishsInClip << "/" << fishsSpare;
				fishText.setString(ssFish.str());

				// Update the score text
				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());

				// Update the high score text
				ssHiScore << "High Score:" << hiScore;
				hiScoreText.setString(ssHiScore.str());

				// Update the wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				// Update the high score text
				ssKittiesAlive << "Kitties:" << numKittiesAlive;
				kittyRemainingText.setString(ssKittiesAlive.str());

				framesSinceLastHUDUpdate = 0;
				timeSinceLastUpdate = Time::Zero;
			}// End HUD update

		}// End updating the scene

		 /*
		 **************
		 Draw the scene
		 **************
		 */

		if (state == State::PLAYING)
		{
			window.clear();

			// set the mainView to be displayed in the window
			// And draw everything related to it
			window.setView(mainView);

			// Draw the background
			window.draw(background, &textureBackground);

			// Draw the cats
			for (int i = 0; i < numKitties; i++)
			{
				window.draw(kitties[i].getSprite());
			}

			for (int i = 0; i < 100; i++)
			{
				if (fishs[i].isInFlight())
				{
					window.draw(fishs[i].getShape());
				}
			}

			// Draw the player
			window.draw(player.getSprite());

			// Draw the pickups is currently spawned
			if (fishPickup.isSpawned())
			{
				window.draw(fishPickup.getSprite());
			}
			if (candyPickup.isSpawned())
			{
				window.draw(candyPickup.getSprite());
			}

			//Draw the crosshair
			window.draw(spriteCrosshair);

			// Switch to the HUD view
			window.setView(hudView);

			// Draw all the HUD elements
			window.draw(spriteFishIcon);
			window.draw(fishText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(candyBar);
			window.draw(waveNumberText);
			window.draw(kittyRemainingText);
		}

		if (state == State::LEVELING_UP)
		{

			window.draw(spriteChoiceOption);
			window.draw(spritechooseLevelIcon);
			window.draw(spritelevel1Icon);
			window.draw(spritelevel2Icon);
			window.draw(spritelevel3Icon);
			window.draw(spritelevel4Icon);
			window.draw(spritelevel5Icon);

		}

		if (state == State::PAUSED)
		{
			window.draw(pausedText);

			window.draw(pausedText1);

			window.draw(spritePausedIcon);
		}

		if (state == State::GAME_OVER)
		{
			sf::Event event;

			while (window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::KeyReleased:
					switch (event.key.code)
					{
					case sf::Keyboard::Up:
						menu.MoveUp();
						break;

					case sf::Keyboard::Down:
						menu.MoveDown();
						break;

					case sf::Keyboard::Return:
						switch (menu.GetPressedItem())
						{
						case 0:
							//std::cout << "Play button has been pressed" << std::endl;
							break;
						case 1:
							//std::cout << "Option button has been pressed" << std::endl;
							break;
						case 2:
							window.close();
							break;
						}

						break;
					}

					break;
				case sf::Event::Closed:
					window.close();
					break;


				}
			}
			///AIICICICICICI

			//window.draw(scoreText);
			//menu.draw(window);
			//window.draw(hiScoreText);

			window.clear();
			//play the background music

			//backgroundmusic.play();

			if (!music.openFromFile("sound/background.wav"))
				return -1; // error
			music.play();

			window.draw(spriteGameOver);

			window.draw(spritePlayIcon);

			window.draw(spriteQuitIcon);

			window.draw(spriteKittyRushIcon);

			//window.draw(gameOverText);

			//menu.draw(window);

			//window.display();

			// ENET
			//window.draw(enemy);
			// ENET
		}


		// ENET
		window.draw(enemy);
		// ENET


		window.display();

	}// End game loop


	// ENET
	/* We delete newPosition, destroy the client instance, and deinitialise ENet. */

	//delete newPosition;

	enet_host_destroy(server);
	atexit(enet_deinitialize);
	// ENET

	return 0;
}