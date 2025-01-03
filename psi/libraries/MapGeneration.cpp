#include "MapGeneration.hpp"

//Values for level generation
//Uwaga: Idiotootpornosc nie wprowadzona, zabawa dalej ryzykowna

//Starting point for first rectangle
constexpr int startX = 3;
constexpr int startY = 3;

//Min and max width/height of rectangles
constexpr int rectMin = 5;
constexpr int rectMax = 11;

//Generates map from seed
//Tiles:
// 0 - background
// 1 - guide tiles
// 2 - path
//TODO: rozne typy generowania sie mapy

void MapGeneration::resetLevel(std::vector<std::vector<int>>& level2D)
{
	// Resetowanie poziomu
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			level2D[i][j] = 0;
		}
	}
}

void MapGeneration::generateRectangle(std::vector<std::vector<int>>& level2D, int x, int y, int width, int height)
{
	for (int i = x; i < x + width; ++i)
	{
		for (int j = y; j < y + height; ++j)
		{
			if (i < WIDTH && j < HEIGHT)  // Sprawdzenie granic mapy
			{
				level2D[i][j] = 1;
			}
		}
	}
}

void MapGeneration::fillGaps(std::vector<std::vector<int>>& level2D)
{
	for (int i = 1; i < WIDTH - 1; ++i)
	{
		for (int j = 1; j < HEIGHT - 1; ++j)
		{
			if (level2D[i][j] == 0)
			{
				if (level2D[i - 1][j] == 1 && level2D[i + 1][j] == 1)
				{
					level2D[i][j] = 1;
				}
				if (level2D[i][j - 1] == 1 && level2D[i][j + 1] == 1)
				{
					level2D[i][j] = 1;
				}
			}
		}
	}
}

void MapGeneration::randomFlipAndRotateMap(std::vector<std::vector<int>>& level2D, std::mt19937& generator)
{
	// Losowanie, czy wykonujemy flip horyzontalny, wertykalny, czy obie operacje
	std::uniform_int_distribution<int> flipDist(0, 3); // 0 - brak flipu, 1 - flip horyzontalny, 2 - flip wertykalny, 3 - oba flipy
	int flipOption = flipDist(generator);

	// Flip horyzontalny
	if (flipOption == 1 || flipOption == 3)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			for (int y = 0; y < HEIGHT / 2; ++y)
			{
				std::swap(level2D[x][y], level2D[x][HEIGHT - y - 1]);
			}
		}
	}

	// Flip wertykalny
	if (flipOption == 2 || flipOption == 3)
	{
		for (int x = 0; x < WIDTH / 2; ++x)
		{
			for (int y = 0; y < HEIGHT; ++y)
			{
				std::swap(level2D[x][y], level2D[WIDTH - x - 1][y]);
			}
		}
	}

	// Losowy obrót o 90, 180 lub 270 stopni
	std::uniform_int_distribution<int> rotationDist(0, 3); // 0 - 0°, 1 - 90°, 2 - 180°, 3 - 270°
	int degrees = rotationDist(generator);

	// Zmieniamy mapê na obrócon¹ o 90, 180 lub 270 stopni
	std::vector<std::vector<int>> tempMap = level2D;

	switch (degrees)
	{
	case 1: // 90°
		for (int x = 0; x < WIDTH; ++x)
		{
			for (int y = 0; y < HEIGHT; ++y)
			{
				level2D[y][WIDTH - x - 1] = tempMap[x][y];
			}
		} break;
	case  2: // 180°
		for (int x = 0; x < WIDTH; ++x)
		{
			for (int y = 0; y < HEIGHT; ++y)
			{
				level2D[WIDTH - x - 1][HEIGHT - y - 1] = tempMap[x][y];
			}
		} break;
	case 3: // 270°
		for (int x = 0; x < WIDTH; ++x)
		{
			for (int y = 0; y < HEIGHT; ++y)
			{
				level2D[HEIGHT - y - 1][x] = tempMap[x][y];
			}
		} break;
	}
}

void MapGeneration::pathHelper(std::vector<std::vector<int>>& level2D)
{
	for (int x = 0; x < WIDTH; ++x)
	{
		for (int y = 0; y < HEIGHT; ++y)
		{
			// Sprawdzamy, czy pole jest puste (0)
			if (level2D[x][y] == 0)
			{
				bool hasNeighbor = false;

				// Sprawdzamy s¹siadów (8 kierunków)
				for (int dx = -1; dx <= 1; ++dx)
				{
					for (int dy = -1; dy <= 1; ++dy)
					{
						// Pomijamy siebie (s¹siad - 0,0)
						if (dx == 0 && dy == 0)
							continue;

						// Obliczamy wspó³rzêdne s¹siada
						int nx = x + dx;
						int ny = y + dy;

						// Sprawdzamy, czy s¹siad znajduje siê w granicach mapy
						if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT)
						{
							// Jeœli s¹siad ma wartoœæ 1, zmieniamy bie¿¹ce pole na 2
							if (level2D[nx][ny] == 1)
							{
								hasNeighbor = true;
								break;
							}
						}
					}
					if (hasNeighbor)
						break;
				}

				// Jeœli mamy s¹siada o wartoœci 1, zmieniamy pole na 2
				if (hasNeighbor)
				{
					level2D[x][y] = 2;
				}
			}
		}
	}
}

void MapGeneration::collectPathPositions(std::vector<std::vector<int>>& level2D, std::vector<sf::Vector2i>& positions)
{
	for (int x = 0; x < WIDTH; ++x)
	{
		for (int y = 0; y < HEIGHT; ++y)
		{
			if (level2D[x][y] == 2) // Jeœli pole jest oznaczone jako œcie¿ka
			{
				positions.emplace_back(x, y);
			}
		}
	}
}

void MapGeneration::generatePath(std::vector<std::vector<int>>& level2D, std::vector<sf::Vector2i>& path, std::mt19937& generator)
{
	// Wype³nienie mapy œcie¿k¹
	pathHelper(level2D);

	// Zbieranie pozycji œcie¿ki
	std::vector<sf::Vector2i> pathPositions;
	collectPathPositions(level2D, pathPositions);

	// Jeœli mamy dostêpne pozycje, losujemy jedn¹ z nich
	if (!pathPositions.empty())
	{
		std::uniform_int_distribution<int> distribution(0, static_cast<int>(pathPositions.size() - 1));
		int randomIndex = distribution(generator);

		// Dodajemy losowy punkt do œcie¿ki
		path.push_back(pathPositions[randomIndex]);
	}

	sf::Vector2i currentPos = path[0];
	sf::Vector2i previousPos = currentPos;

	// Definicja kierunków: Dó³, Góra, Prawo, Lewo
	std::vector<sf::Vector2i> directions = {
		{0, 1},  // Dó³
		{0, -1}, // Góra
		{1, 0},  // Prawo
		{-1, 0}  // Lewo
	};

	// Tworzymy œcie¿kê
	do
	{
		// Sprawdzamy dostêpne kierunki
		for (const auto& direction : directions)
		{
			// Nowa pozycja
			sf::Vector2i nextPos = currentPos + direction;

			// Sprawdzamy, czy ruch nie wykracza poza mapê i czy pole jest oznaczone jako œcie¿ka (2)
			if (nextPos.x >= 0 && nextPos.x < WIDTH && nextPos.y >= 0 && nextPos.y < HEIGHT && level2D[nextPos.x][nextPos.y] == 2)
			{
				// Jeœli to nie jest ta sama pozycja, z której przyszliœmy (unikamy cofania)
				if (nextPos != previousPos)
				{
					path.push_back(nextPos);  // Dodajemy now¹ pozycjê do œcie¿ki
					previousPos = currentPos; // Ustawiamy poprzedni¹ pozycjê
					currentPos = nextPos;     // Aktualizujemy bie¿¹c¹ pozycjê
					break;                    // Przechodzimy do nastêpnego kroku
				}
			}
		}
	} while (currentPos != path[0]); // Powtarzamy, dopóki nie wrócimy do pocz¹tku œcie¿ki

}

void MapGeneration::generate(uint_least32_t seed, int* level, std::vector<sf::Vector2i>& path)
{
	std::cout << gradient("FF70A6", "70D6FF" , "Generating new map with seed: " +  std::to_string(seed)) << "\n";
	// Tworzymy mapê 2D, która bêdzie u¿ywana do generowania
	std::vector<std::vector<int>> level2D(WIDTH, std::vector<int>(HEIGHT, 0));

	// Resetowanie poziomu
	resetLevel(level2D);
	path.clear();

	// Inicjalizacja generatora losowego
	std::mt19937 generator(seed);

	std::uniform_int_distribution<uint_least32_t> dimentions(rectMin, rectMax);
	std::uniform_int_distribution<uint_least32_t> startpoint(startX, startY);

	//Number of iterated rectangles
	std::uniform_int_distribution<uint_least32_t> numberOfRectDist(4, 7);
	int numberOfRect = numberOfRectDist(generator);

	// Tworzenie pocz¹tkowego prostok¹ta
	int x = startpoint(generator);
	int y = startpoint(generator);
	int xx = dimentions(generator);
	int yy = dimentions(generator);

	generateRectangle(level2D, x, y, xx, yy);

	// Tworzenie kolejnych prostok¹tów
	for (int k = 0; k < numberOfRect; ++k)
	{
		std::uniform_int_distribution<uint_least32_t> newPointX(x + 2, x + xx);
		std::uniform_int_distribution<uint_least32_t> newPointY(y + 2, y + yy);

		x = newPointX(generator);
		y = newPointY(generator);

		xx = dimentions(generator);
		yy = dimentions(generator);

		generateRectangle(level2D, x, y, xx, yy);
	}

	// Wype³nianie luk miêdzy prostok¹tami
	fillGaps(level2D);

	// Losowe obracanie i odbijanie mapy
	randomFlipAndRotateMap(level2D, generator);

	// Generowanie œcie¿ki
	generatePath(level2D, path, generator);

	// Na koniec zapisujemy z powrotem mapê 2D do 1D tablicy
	for (int i = 0; i < WIDTH; ++i)
	{
		for (int j = 0; j < HEIGHT; ++j)
		{
			level[i * HEIGHT + j] = level2D[i][j];
		}
	}
}