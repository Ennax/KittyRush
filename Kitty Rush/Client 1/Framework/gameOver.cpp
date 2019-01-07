#include "gameOver.h"
#include "KittyArena.h"
#include "TextureHolder.h"

gameOver::gameOver(float width, float height)
{
	if (!font.loadFromFile("fonts/HelloCute.ttf"))
	{
		// For handling the error
	}

	menu[0].setFont(font);
	menu[0].setColor(sf::Color::Magenta);
	menu[0].setString("Press ENTER to P L A Y");
	//Changed the size, so it can be seenable.
	menu[0].setCharacterSize(70);
	menu[0].setPosition(sf::Vector2f(width / 2, height / (MAX_NUMBER_OF_ITEMS + 1) * 1));

	/*menu[1].setFont(font);
	menu[1].setColor(sf::Color::White);
	menu[1].setString("O P T I O N S");
	menu[1].setCharacterSize(70);
	menu[1].setPosition(sf::Vector2f(width / 2, height / (MAX_NUMBER_OF_ITEMS + 1) * 2)); */



	menu[2].setFont(font);
	menu[2].setColor(sf::Color::White);
	menu[2].setString("Press ESC to E X I T");
	menu[2].setCharacterSize(70);
	menu[2].setPosition(sf::Vector2f(width / 2, height / (MAX_NUMBER_OF_ITEMS + 1) * 2));

	selectedItemIndex = 0;
}


gameOver::~gameOver()
{
}

void gameOver::draw(sf::RenderWindow &window)
{
	for (int i = 0; i < MAX_NUMBER_OF_ITEMS; i++)
	{
		window.draw(menu[i]);
	}
}

void gameOver::MoveUp()
{
	if (selectedItemIndex - 1 >= 0)
	{
		menu[selectedItemIndex].setColor(sf::Color::White);
		selectedItemIndex--;
		menu[selectedItemIndex].setColor(sf::Color::Magenta);
	}
}

void gameOver::MoveDown()
{
	if (selectedItemIndex + 1 < MAX_NUMBER_OF_ITEMS)
	{
		menu[selectedItemIndex].setColor(sf::Color::White);
		selectedItemIndex++;
		menu[selectedItemIndex].setColor(sf::Color::Magenta);
	}
}