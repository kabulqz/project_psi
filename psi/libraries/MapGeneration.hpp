#pragma once

#include "TileMap.hpp"
#include "Save.hpp"

class MapGeneration
{
	// Map Generation

	// Level Generation
	static void resetLevel(std::vector<std::vector<int>>& level2D);
	static void generateRectangle(std::vector<std::vector<int>>& level2D, int x, int y, int width, int height);
	static void fillGaps(std::vector<std::vector<int>>& level2D);
	static void pathHelper(std::vector<std::vector<int>>& level2D);
	static void collectPathPositions(std::vector<std::vector<int>>& level2D, std::vector<sf::Vector2i>& positions);
	static void generatePath(std::vector<std::vector<int>>& level2D, std::vector<sf::Vector2i>& path, std::mt19937& generator);
public:
	static void generate(uint_least32_t seed, int* level, std::vector<sf::Vector2i>& p);
};