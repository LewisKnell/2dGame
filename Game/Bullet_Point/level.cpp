#include "level.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <LKengine\myErrors.h>
#include <LKengine\resourceManager.h>


Level::Level(const std::string& fileName)
{
	std::ifstream file;
	file.open(fileName);

	//error checking
	if (file.fail())
	{
		LKengine::fatalError("Failed to open " + fileName);
	}

	bool countColumns = true;

	std::string line;

	// Go throught getting each line of the input file
	while (std::getline(file, line))
	{
		std::vector<std::string> row;
		std::stringstream iss(line);
		int c = 0;
		std::string val;

		numRows++;

		// Go along the row and take the value up to the "," then put that into the vector as the value
		while (getline(iss, val, ','))
		{
			// no need for converting as you are reading string.
			row.push_back(val);
			numTiles++;

			if (countColumns == true)
				numColumns++;
		}
		m_levelData2.push_front(row);
		countColumns = false;
	}

	std::cout << " Data loaded \n";

	// Loop from 0 -> deque size - 1
	for (unsigned int i = 0; i < m_levelData2.size(); i++)
	{
		// Assign data to vector, at(i) is pretty much equivalent with [i]
		m_levelData.push_back(m_levelData2.at(i));
	}

	//std::cout << "Tile in [1][1] is: " << m_levelData[1][1] << std::endl;

	std::cout << "Data loaded: \n";
	std::cout << numTiles << " tiles \n";
	std::cout << numRows << " Rows \n";
	std::cout << numColumns << " Columns \n";

	////print it out
	//for (int i = 0; i < m_levelData.size(); i++) {

	//	std::cout << m_levelData[i][i] << std::endl;
	//}

	std::cout << "Data displayed \n";

	m_spriteBatch.init();
	m_spriteBatch.begin();

	//glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	LKengine::ColorRGBA8 whiteColor(255, 255, 255, 255);


	// Load level sprite sheet
	LKengine::GLTexture texture = LKengine::ResourceManager::getTexture("Textures/Levels/aztec.png");
	// Set the size of level sprite sheet
	m_levelTexture.init(texture, glm::ivec2(16, 8));


	for (int y = 0; y < m_levelData.size(); y++)
	{
		for (int x = 0; x < m_levelData[y].size(); x++)
		{
			//grab the tile
			std::string tile = m_levelData[y][x];

			//get dest rect
			glm::vec4 destRect(x * TILE_WIDTH, y * TILE_WIDTH, TILE_WIDTH, TILE_WIDTH);

			glm::vec4 uvRect;

			// ***** Ground textures *****
			// Make sure to change the tiles to a '.' so we dont collide with this tile
			if (tile == "39")
			{
				m_levelData[y][x] = '.'; //change tile to air so we don't collide
				m_tileIndex = 39;
				uvRect = m_levelTexture.getUVsTopLeft(m_tileIndex);

				m_spriteBatch.draw(destRect,
					uvRect,
					m_levelTexture.texture.id,
					0.0f,
					whiteColor);

			}

			// ***** Wall textures *****
			if (tile == "1")
			{
				m_tileIndex = 1;
				uvRect = m_levelTexture.getUVsTopLeft(m_tileIndex);

				m_spriteBatch.draw(destRect,
					uvRect,
					m_levelTexture.texture.id,
					0.0f,
					whiteColor);

			}


		}
	}

	m_spriteBatch.end();


	// Player start position
	m_startPlayerPos.x = 2 * TILE_WIDTH;
	m_startPlayerPos.y = 2 * TILE_WIDTH;

	// Zombie start position
	//m_zomebieStartPos.emplace_back(37 * TILE_WIDTH, 37 * TILE_WIDTH);
	m_zomebieStartPos.emplace_back(7 * TILE_WIDTH, 3 * TILE_WIDTH);

	
}


Level::~Level()
{
}


void Level::draw()
{
	m_spriteBatch.renderBatch();

}


void Level::drawDebug(LKengine::DebugRenderer& debugRenderer)
{

	for (int y = 0; y < m_levelData.size(); y++)
	{
		for (int x = 0; x < m_levelData[y].size(); x++)
		{
			if (m_levelData[y][x] != ".")
			{
				LKengine::ColorRGBA8 color(255, 255, 255, 255);
				// Draw box
				glm::vec4 destRect;
				destRect.x = x * TILE_WIDTH;
				destRect.y = y * TILE_WIDTH;
				destRect.z = TILE_WIDTH;
				destRect.w = TILE_WIDTH;
				debugRenderer.drawBox(destRect, LKengine::ColorRGBA8(255, 255, 255, 255), 0);
			}
		}
	}

}