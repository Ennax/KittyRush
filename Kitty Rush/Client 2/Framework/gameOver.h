#pragma warning(disable : 4996)
#pragma once


#include "SFML/Graphics.hpp"

#define MAX_NUMBER_OF_ITEMS 3

class gameOver
{
public:
	gameOver(float width, float height);
	~gameOver();

	void draw(sf::RenderWindow &window);
	void MoveUp();
	void MoveDown();
	int GetPressedItem() { return selectedItemIndex; }

private:
	int selectedItemIndex;
	sf::Font font;
	sf::Text menu[MAX_NUMBER_OF_ITEMS];

};