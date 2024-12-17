#include "MapGeneration.hpp"

//Values for level generation
//Uwaga: Idiotootpornosc nie wprowadzona, zabawa dalej ryzykowna

//Starting point for first rectangle
constexpr int startX = 3;
constexpr int startY = 5;

//Min and max width/height of rectangles
constexpr int rectMin = 5;
constexpr int rectMax = 11;

//Number of iterated rectangles
constexpr int numberOfRect = 5;

enum Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

//Generates map from seed
//Tiles:
// 0 - background
// 1 - guide tiles
// 2 - path
//TODO: rozne typy generowania sie mapy

void generate(uint_least32_t seed, int* level, std::vector<sf::Vector2i>& p)
{
	//Map reset
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			level[i * WIDTH + j] = 0;
		}
	}

	//Setup for RNG
	using u32 = uint_least32_t;
	using engine = std::mt19937;

	engine generator(seed);

	std::uniform_int_distribution< u32 > startpoint(startX, startY);
	std::uniform_int_distribution< u32 > dimentions(rectMin, rectMax);

	//Generating shapes
	int x = startpoint(generator);
	int y = startpoint(generator);

	int xx = dimentions(generator);
	int yy = dimentions(generator);

	p.emplace_back(x, y - 1); 
	level[p[0].x * WIDTH + p[0].y] = 2;

	//Starting shape
	for (int i = x; i < x + xx; i++)
	{
		for (int j = y; j < y + yy; j++)
		{
			level[i * WIDTH + j] = 1;
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
				level[i * WIDTH + j] = 1;
			}
		}
	}

	//Checks for one tile gaps between shapes
	//Needs some optimalization, but thats problem for future me
	for (int i = 2; i < WIDTH; i++)
	{
		for (int j = 2; j < HEIGHT; j++)
		{
			if (level[i * WIDTH + j] == 0)
			{
				if (level[(i - 1) * WIDTH + j] == 1 && level[(i + 1) * WIDTH + j] == 1)
				{
					level[i * WIDTH + j] = 1;
				}
				if (level[i * WIDTH + (j - 1)] == 1 && level[i * WIDTH + (j + 1)] == 1)
				{
					level[i * WIDTH + j] = 1;
				}
			}
		}
	}

	//Generating path
	//Finally, this is not near as retarted as last version
	//And it also fills path vector correctly :)

	Direction dir = UP;
	sf::Vector2i current = p.back();
	int i = 0;
	int max = 1000;

	do
	{
		switch (dir)
		{
		case UP:
			//std::cout << "Checking UP\n";
			if (level[(current.x - 1) * WIDTH + current.y] == 1)
			{
				level[(current.x) * WIDTH + current.y] = 2;
				p.emplace_back(current.x, current.y - 1);
				dir = LEFT;
				//std::cout << "DIR LEFT\n";
			}
			else
			{
				if (level[current.x * WIDTH + (current.y + 1)] == 1)
				{
					level[(current.x) * WIDTH + current.y] = 2;
					p.emplace_back(current.x - 1, current.y);
					//std::cout << "DIR STRAIGHT\n";
				}
				else
				{
					level[(current.x) * WIDTH + current.y] = 2;
					p.emplace_back(current.x, current.y + 1);
					dir = RIGHT;
					//std::cout << "DIR RIGHT\n";
				}
			}
			break;
		case DOWN:
			//std::cout << "Checking DOWN\n";
			if (level[(current.x + 1) * WIDTH + current.y] == 1)
			{
				level[(current.x) * WIDTH + current.y] = 2;
				p.emplace_back(current.x, current.y + 1);
				dir = RIGHT;
				//std::cout << "DIR RIGHT\n";
			}
			else
			{
				if (level[current.x * WIDTH + (current.y - 1)] == 1)
				{
					level[(current.x) * WIDTH + current.y] = 2;
					p.emplace_back(current.x + 1, current.y);
					//std::cout << "DIR STRAIGHT\n";
				}
				else
				{
					level[(current.x) * WIDTH + current.y] = 2;
					p.emplace_back(current.x, current.y - 1);
					dir = LEFT;
					//std::cout << "DIR LEFT\n";
				}
			}
			break;
		case LEFT:
			//std::cout << "Checking LEFT\n";
			if (level[current.x * WIDTH + (current.y - 1)] == 1)
			{
				level[(current.x) * WIDTH + current.y] = 2;
				p.emplace_back(current.x + 1, current.y);
				dir = DOWN;
				//std::cout << "DIR DOWN\n";
			}
			else
			{
				if (level[(current.x - 1) * WIDTH + current.y] == 1)
				{
					level[(current.x) * WIDTH + current.y] = 2;
					p.emplace_back(current.x, current.y - 1);
					//std::cout << "DIR STRAIGHT\n";
				}
				else
				{
					level[(current.x) * WIDTH + current.y] = 2;
					p.emplace_back(current.x - 1, current.y);
					dir = UP;
					//std::cout << "DIR UP\n";
				}
			}
			break;
		case RIGHT:
			//std::cout << "Checking UP\n";
			if (level[current.x * WIDTH + (current.y + 1)] == 1)
			{
				level[(current.x) * WIDTH + current.y] = 2;
				p.emplace_back(current.x - 1, current.y);
				dir = UP;
				//std::cout << "DIR UP\n";
			}

			else
			{
				if (level[(current.x + 1)* WIDTH + current.y] == 1)
				{
					level[(current.x) * WIDTH + current.y] = 2;
					p.emplace_back(current.x, current.y + 1);
					//std::cout << "DIR STRAIGHT\n";
				}
				else
				{
					level[(current.x) * WIDTH + current.y] = 2;
					p.emplace_back(current.x + 1, current.y);
					dir = DOWN;
					//std::cout << "DIR DOWN\n";
				}
			}
			break;
		default:
			std::cout << "\n-ERROR-\n";
		}
		current = p.back();
		//jesli to czytasz, to pamietaj: jesli ktos zaproponuje zabawe na takich tablicach, pierdolnij mu w leb
	} while (++i < max &&p.back() != p[0]);

	for (auto& path : p)
	{
		std::swap(path.x, path.y);
	}
}