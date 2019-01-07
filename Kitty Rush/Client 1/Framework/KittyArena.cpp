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
	// ENET SETUP BEGIN // ENET SETUP BEGIN // ENET SETUP BEGIN // ENET SETUP BEGIN // ENET SETUP BEGIN // ENET SETUP BEGIN //
	//

	// We create an instance for the PLAYER 2! 

	sf::RectangleShape enemy;

	enemy.setSize(sf::Vector2f(100.0f, 100.0f));

	enemy.setPosition(sf::Vector2f(600.0f, 100.0f));

	sf::Texture testTexture;

	if (!testTexture.loadFromFile("graphics/player1.png"))
	{
		cout << "Texture did not load!" << "\n";
	}
	enemy.setTexture(&testTexture);
	// We create an instance for the PLAYER 2!

	if (enet_initialize() != 0)
	{
		cout << "Enet failed to initialise!" << "\n\n";
	}

	/* Much of this will be familiar from the server code. What differs
	is the inclusion of an Peer. This is basically ENet's way of tracking
	what you're connected to. Since our Client will be initialising the
	connection, they need a peer to store information about the server they're
	connecting to. */

	ENetAddress address;
	ENetHost* client;
	ENetPeer* peer;
	ENetEvent enetEvent;

	client = enet_host_create(NULL, 1, 2, 0, 0);

	if (client == NULL)
	{
		cout << "Client failed to initialise!" << "\n\n";
	}

	enet_address_set_host(&address, "localhost");
	address.port = 1234;

	/* Now we're trying to connect to the server. We include who we are,
	the address to establish the connection to, the number of channels, and a
	generic data variable we can use to say something to the server (currently 0). */

	peer = enet_host_connect(client, &address, 2, 0);

	if (peer == NULL) {
		cout << "No available peers for initializing an ENet connection.\n";
	}

	/* To make our lives a little easier, we declare this newPosition variable on
	the heap. It's where the data from our received packet is going to be stored,
	if it reaches us. */

	Vector2ENET* newPosition = new Vector2ENET;


	//
	// ENET SETUP END // ENET SETUP END // ENET SETUP END // ENET SETUP END // ENET SETUP END // ENET SETUP END 
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
						if (peer != NULL)
						{
							enet_peer_disconnect_now(peer, 0);
						}
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

		// ENET
		while (enet_host_service(client, &enetEvent, 0) > 0)
		{
			switch (enetEvent.type) {

				/* Again, we're reacting based on the detected event type. In this case, it's
				ENET_EVENT_TYPE_RECEIVE, which means our client has recieved a packet from a
				peer (in this case, the server). After quickly outputting some debug text to
				console to confirm packet receipt, what happens next is the key part.

				Our packet has some intrinsic variables - its data (what it's storing) and its
				dataLength (how much data it's storing). In this case, since we KNOW the packet
				is a Vector2, we can use the memcpy function fairly easily. This is a basic C++
				function which copies a given amount of data from one location to another. In
				this case, it copies TO newPosition FROM data, and it copies an amount dataLength.

				Given what we know about pointer arithmetic, it should be obvious to us that we
				can make these packets more sophisticated. We can make huge packets which hold
				many different kinds of data. We simply include an enumerator at the beginning of
				each data segment, saying what data type it is, and either copy that much data over
				into a variable of that type, or include as the next element of a packet the amount
				of data this variable type needs to copy. This is particularly useful when it comes
				to Part 2 of the coursework, where 'level data' is likely very different to the
				'physics data' you'll have been transmitting for Part 1. */

			case ENET_EVENT_TYPE_RECEIVE:
				cout << "Packet received!\n";
				memcpy(newPosition, enetEvent.packet->data, enetEvent.packet->dataLength);
				cout << newPosition->x << "," << newPosition->y << "\n";
				enemy.setPosition(sf::Vector2f(newPosition->x, newPosition->y));
				break;
			}
		}
		// ENET
		 // Handle the player quitting
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			// ENET
			if (peer != NULL)
			{
				enet_peer_disconnect_now(peer, 0);
			}
			// ENET
			window.close();
		}

		// Handle controls while playing
		if (state == State::PLAYING)
		{
			
			// Handle the pressing and releasing of the WASD keys
			if (Keyboard::isKeyPressed(Keyboard::W))
			{
				player.moveUp();
			}
			else
			{
				player.stopUp();
			}

			if (Keyboard::isKeyPressed(Keyboard::S))
			{
				player.moveDown();
			}
			else
			{
				player.stopDown();
			}

			if (Keyboard::isKeyPressed(Keyboard::A))
			{
				player.moveLeft();
			}
			else
			{
				player.stopLeft();
			}

			if (Keyboard::isKeyPressed(Keyboard::D))
			{
				player.moveRight();
			}
			else
			{
				player.stopRight();
			}

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

	delete newPosition;

	enet_host_destroy(client);
	atexit(enet_deinitialize);
	// ENET

	return 0;
}