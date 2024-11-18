#include "GameBoardState.hpp"
#include "Game.hpp"

//Values for level generation
//Uwaga: Bawisz siê nimi na w³asne ryzyko. Jak przegniesz pa³e GRA SIÊ WYWALI.
//		 Radzê trzymaæ siê rozs¹dnych wartoœci dopóki tego nie ogarnê.

//Width and height of a map
const int width = 60;
const int height = 60;

//Starting point for first rectangle
const int startX = 3;
const int startY = 5;

//Min and max width/height of rectangles
const int rectMin = 5;
const int rectMax = 11;

//Number of iterated rectangles
const int numberOfRect = 5;

GameBoardState::GameBoardState(Game* game) : game(game)
{
	//Jest to rozwi¹zanie. Jakieœ
	//0 - background
	//1 - shapes used for path
	//2 - path

	int level[width * height];

	//Filling array with background
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			level[i + j * width] = 0;
		}
	}

	//Random number generation
	using u32 = uint_least32_t;
	using engine = std::mt19937;

	//std::random_device os_seed;
	//const u32 seed = os_seed();
	Save save = game->getSave();

	engine generator(save.getSeed());
	std::cout << "Level seed: " << save.getSeed() << std::endl;
	std::uniform_int_distribution< u32 > startpoint(startX, startY);
	std::uniform_int_distribution< u32 > dimentions(rectMin, rectMax);

	//Generating shapes
	int x = startpoint(generator);
	int y = startpoint(generator);

	int xx = dimentions(generator);
	int yy = dimentions(generator);

	//Starting shape
	for (int i = x; i < x + xx; i++)
	{
		for (int j = y; j < y + yy; j++)
		{
			level[i + j * width] = 1;
		}
	}

	//Next shapes
	for (int k = 0; k < numberOfRect; k++)
	{
		std::uniform_int_distribution< u32 > newpointX(x + 1, x + xx);
		std::uniform_int_distribution< u32 > newpointY(y + 1, y + yy);

		x = newpointX(generator);
		y = newpointY(generator);

		xx = dimentions(generator);
		yy = dimentions(generator);

		for (int i = x; i < x + xx; i++)
		{
			for (int j = y; j < y + yy; j++)
			{
				level[i + j * width] = 1;
			}
		}
	}

	//Generating path
	//This is retarded
	for (int i = 0; i < width * height; i++)
	{
		if (level[i] == 1)
		{
			if (level[i + 1] == 0)
				level[i + 1] = 2;
			if (level[i - 1] == 0)
				level[i - 1] = 2;
			if (level[i + width] == 0)
				level[i + width] = 2;
			if (level[i - width] == 0)
				level[i - width] = 2;
			if (level[i + width + 1] == 0)
				level[i + width + 1] = 2;
			if (level[i + width - 1] == 0)
				level[i + width - 1] = 2;
			if (level[i - width + 1] == 0)
				level[i - width + 1] = 2;
			if (level[i - width - 1] == 0)
				level[i - width - 1] = 2;
		}
	}

	//Loading map
	if (!map.load("src/img/test_map_1.png", sf::Vector2u(16, 16), level, width, height))
		return;
}
//handler for specific windows to appear in the main frame 
void GameBoardState::handleInput(sf::RenderWindow& window, EventManager& eventManager, SoundManager& soundManager, sqlite3*& database)
{
	while (eventManager.hasEvents())
	{
		sf::Event event = eventManager.popEvent();
		// Handle other events, such as changing the state or closing the window
		// Example: if (event.type == sf::Event::MouseButtonPressed) { ... }
	}
}

//updater for elements corresponding to specific screen
void GameBoardState::update()
{

}

//function rendering screen
void GameBoardState::render(sf::RenderWindow& window)
{
	window.clear();
	//draw elements
	window.draw(map);
	window.display();
}