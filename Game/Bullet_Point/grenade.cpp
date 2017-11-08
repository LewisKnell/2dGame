
#include <LKengine\resourceManager.h>

#include "grenade.h"
#include "level.h"



#include <algorithm>



Grenade::Grenade(std::string name, 
	glm::vec2& position, 
	const glm::vec2& direction, 
	const glm::vec2& destination, 
	LKengine::TileSheet texture,
	int tileID)
{
	m_name = name;
	m_position = position;
	m_direction = direction;
	m_destination = destination;
	m_grenadeTexture = texture;
	m_tileID = tileID;

	m_timer = 5; // Sets how longuntil the grenade explodes

	m_startTime = clock(); //Start time grenade was thrown


}


Grenade::~Grenade()
{
	// Empty
}


bool Grenade::update(const std::vector< std::vector<std::string> >& levelData, float deltaTime)
{
	// Update the seconds passed since grenade was thrown
	m_secondsPassed = (clock() - m_startTime) / CLOCKS_PER_SEC;


	if (collideWithWorld(levelData))
	{
		return collideWithWorld(levelData);
	}
	else
	{
		// Update position of the grenade
		m_position += m_direction * GRENADE_SPEED * deltaTime; // comment this out to debug start position
		return collideWithWorld(levelData);
	}


}


void Grenade::draw(LKengine::SpriteBatch& spriteBatch)
{
	glm::vec4 destRect(m_position.x/* - GRENADE_WIDTH / 2*/,
		m_position.y/* - GRENADE_WIDTH / 2*/,
		GRENADE_WIDTH,
		GRENADE_WIDTH);

	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	LKengine::ColorRGBA8 bulletColor;
	bulletColor = LKengine::ColorRGBA8(75, 75, 75, 255);

	spriteBatch.draw(destRect, uvRect, m_grenadeTexture.texture.id, 0.0f, bulletColor);
}


void Grenade::drawDebug(LKengine::DebugRenderer& debugRenderer)
{
	LKengine::ColorRGBA8 color(255, 255, 255, 255);

	// Draw box
	glm::vec4 destRect;
	destRect.x = m_position.x;
	destRect.y = m_position.y;
	destRect.z = GRENADE_WIDTH;
	destRect.w = GRENADE_WIDTH;
	debugRenderer.drawBox(destRect, LKengine::ColorRGBA8(255, 255, 255, 255), 0);
}


bool Grenade::collideWithWorld(const std::vector< std::vector<std::string> >& levelData)
{
	// Set the min and max X and Y values
	int minX = floor(m_position.x / (float)TILE_WIDTH);
	int maxX = floor((m_position.x + GRENADE_WIDTH) / (float)TILE_WIDTH);

	int minY = floor(m_position.y / (float)TILE_WIDTH);
	int maxY = floor((m_position.y + GRENADE_WIDTH) / (float)TILE_WIDTH);


	//if outside world, return true (delete bullet)
	if (minX < 0 || minX >= levelData[0].size() ||
		minY < 0 || minY >= levelData.size())
	{
		return true;
	}

	// Bottom Left corner check
	if (levelData[minY][minX] != ".")
	{
		return true;
	}
	// Bottom Right corner check
	if (levelData[minY][maxX] != ".")
	{
		return true;
	}
	// Top Right corner check
	if (levelData[maxY][maxX] != ".")
	{
		return true;
	}
	// Top Left corner check
	if (levelData[maxY][minX] != ".")
	{
		return true;
	}

	// If no collision, return false
	return false;
}
