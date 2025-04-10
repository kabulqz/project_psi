#pragma once
#pragma warning(disable : 4244)

#include "Settings.hpp"

//To, co tu widzisz nie jest moj� prac�. Nie jestem nawet do ko�ca pewny jak to dzia�a.
//Ale dzia�a. I ta�czy�em wok� tego, �eby dzia�a�o jak chc�.
class TileMap : public sf::Drawable, public sf::Transformable
{
	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
	std::vector<sf::Vector2i> path;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		states.texture = &m_tileset;
		target.draw(m_vertices, states);
	}

public:
	bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height)
	{
		// load the tileset texture
		if (!m_tileset.loadFromFile(tileset))
			return false;

		// resize the vertex array to fit the level size
		m_vertices.setPrimitiveType(sf::Quads);
		m_vertices.resize(width * height * 4);

		for (unsigned int i = 0; i < width; i++)
		{
			for (unsigned int j = 0; j < height; j++)
			{
				int tileNumber = tiles[i + j * width];

				// find its position in the tileset texture
				int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
				int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

				// get a pointer to the current tile's quad
				sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

				// define its 4 corners
				quad[0].position = sf::Vector2f(j * tileSize.x, i * tileSize.y); // Zamiana x z y
				quad[1].position = sf::Vector2f((j + 1) * tileSize.x, i * tileSize.y);
				quad[2].position = sf::Vector2f((j + 1) * tileSize.x, (i + 1) * tileSize.y);
				quad[3].position = sf::Vector2f(j * tileSize.x, (i + 1) * tileSize.y);
				
				// define its 4 texture coordinates
				quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
				quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
				quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
			}
		}
		return true;
	}
};