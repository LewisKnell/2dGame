#pragma once

#include <string>
#include <vector>
#include <deque>

#include <LKengine\spriteBatch.h>
#include <LKengine\tileSheet.h>
#include <LKengine\debugRenderer.h>



const int TILE_WIDTH = 64;

class Level
{
public:
	//load the level
	Level(const std::string& fileName);
	~Level();

	// Draw the level
	void draw();

	void drawDebug(LKengine::DebugRenderer& debugRenderer);

	//getters
	int getWidth() const { return m_levelData[0].size(); }
	int getHeight() const { return m_levelData.size(); }

	int getNumHumans() const { return m_numHumans; }

	const std::vector< std::vector<std::string> >& getLevelData() const { return m_levelData; }
	
	int getLevelWidth() const { return m_levelData[0].size(); }
	int getLevelHeight() const { return m_levelData.size(); }

	glm::vec2 getStartPlayerPos() const { return m_startPlayerPos; }
	const std::vector<glm::vec2>& getZombieStartPos() const { return m_zomebieStartPos; }



	int numTiles;
	int numRows;
	int numColumns;



private:

	int m_mapWidth = 40;
	int m_mapHeight = 40;

	std::vector< std::vector<std::string> > m_levelData;
	std::deque< std::vector<std::string> > m_levelData2;
	
	int m_numHumans = 20;

	LKengine::SpriteBatch m_spriteBatch;
	
	glm::vec2 m_startPlayerPos;
	std::vector<glm::vec2> m_zomebieStartPos;

	LKengine::TileSheet m_levelTexture;
	LKengine::TileSheet m_levelTexture2;
	LKengine::TileSheet m_levelTexture3;
	LKengine::TileSheet m_levelTexture4;

	int m_tileIndex = 0;




};

