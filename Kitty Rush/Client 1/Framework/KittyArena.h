#pragma once
#include "Kitty.h"
using namespace sf;

int createBackground(VertexArray& rVA, IntRect arena);
int createOverLay(VertexArray& rVA, IntRect arena);

Kitty* createHorde(int numKitties, IntRect arena);


