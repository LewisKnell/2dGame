#include "bullet.h"
#include "zombie.h"
#include "level.h"

#include <LKengine\resourceManager.h>
#include <math.h> // For fmod as % operator is only for ints
#include <algorithm> // std::min

#include <iostream> // Delete this when no more debugging needed




Bullet::Bullet(glm::vec2 position, glm::vec2 direction, float damage, float speed) :
	m_direction(direction),
	m_damage(damage),
	m_speed(speed)
{
	m_position.x = position.x + BULLET_RADIUS;
	m_position.y = position.y + BULLET_RADIUS;
}


Bullet::~Bullet()
{
	//Empty
}


bool Bullet::update(const std::vector< std::vector<std::string> >& levelData, float deltaTime)
{
	if (collideWithWorld(levelData))
	{
		return collideWithWorld(levelData);
	}
	else
	{
		m_position += m_direction * m_speed * deltaTime;
		return collideWithWorld(levelData);
	}
}


void Bullet::draw(LKengine::SpriteBatch& spriteBatch)
{
	glm::vec4 destRect(	m_position.x,
						m_position.y, 
						BULLET_RADIUS*2, 
						BULLET_RADIUS*2);

	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	LKengine::ColorRGBA8 bulletColor;
	bulletColor = LKengine::ColorRGBA8(75, 75, 75, 255);

	spriteBatch.draw(destRect, uvRect, LKengine::ResourceManager::getTexture("Textures/circle.png").id, 0.0f, bulletColor);
}


bool Bullet::collideWithEnemies(Enemies* agent)
{
	const float MIN_DISTANCE = ENEMY_RADIUS + BULLET_RADIUS;

	glm::vec2 centrePosA = m_position; // Position of bullet
	glm::vec2 centrePosB = agent->getPosition() + glm::vec2(ENEMY_RADIUS); // Position of pass in enemy

	glm::vec2 distVec = centrePosA - centrePosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;
	if (collisionDepth > 0)
	{
		return true;
	}
	return false;
}


// Check the side the bullet has collided with for bullet impact decal rotation
float Bullet::collisionSide(const std::vector< std::vector<std::string> >& levelData)
{
	glm::ivec2 gridPosition;
	gridPosition.x = floor(m_position.x / TILE_WIDTH);
	gridPosition.y = floor(m_position.y / TILE_WIDTH);

	glm::vec2 bulletPosition;
	bulletPosition.x = m_position.x / TILE_WIDTH;
	bulletPosition.y = m_position.y / TILE_WIDTH;

	// Check whether we need to figure out if a collision has occured (Ignores all ground tiles)
	if (levelData[gridPosition.y][gridPosition.x] != ".")
	{
		// fmod returns floating point remainder. Using instead of a % b which is for int
		float xRemainder = fmod(bulletPosition.x, gridPosition.x);
		float yRemainder = fmod(bulletPosition.y, gridPosition.y);

		// Using fmin & fmax to return a 0 instead of NaN
		// Returns true if it should be left or right side hit as the x values are lower than the y values
		if (std::min(fmax(xRemainder, 0.0f), 1-xRemainder) <= std::min(fmax(yRemainder, 0.0f), 1-yRemainder))
		{
			m_lrHit = true;
		}
		// Returns true if it should be top or bottom side hit as the x values are lower than the y values
		else if (std::min(fmax(yRemainder, 0.0f), 1 - yRemainder) <= std::min(fmax(xRemainder, 0.0f), 1 - xRemainder))
		{
			m_tbHit = true;
		}


		if (m_lrHit == true)
		{	
			// If a side is hit, then check if left or right side was hit
			// Right side collision check
			if (bulletPosition.x < gridPosition.x + 1 && bulletPosition.x > gridPosition.x + 0.5)
			{
				return 90;
			}
			// Left side collision check
			if (bulletPosition.x > gridPosition.x && bulletPosition.x < gridPosition.x + 0.5)
			{
				return 270;
			}
		}
		else if (m_tbHit == true)
		{
			// If top or bottom is hit, then check if top or bottom side was hit
			// Top side collision check
			if (bulletPosition.y < gridPosition.y + 1 && bulletPosition.y > gridPosition.y + 0.5)
			{
				return 180;
			}
			// Bottom side collision check
			if (bulletPosition.y > gridPosition.y && bulletPosition.y < gridPosition.y + 0.5)
			{
				return 0;
			}
		}	
		else
		{	// Make decal draw diagonal if all checks fail
			std::cout << "ALL SIDE CHECKS FAILED! \n";
			return 45;
		}
	}


}


bool Bullet::collideWithWorld(const std::vector< std::vector<std::string> >& levelData)
{
	glm::ivec2 gridPosition;
	gridPosition.x = floor(m_position.x / TILE_WIDTH);
	gridPosition.y = floor(m_position.y / TILE_WIDTH);

	//if outside world, return true (delete bullet)
	if (gridPosition.x < 0 || gridPosition.x >= levelData[0].size() ||
		gridPosition.y < 0 || gridPosition.y >= levelData.size())
	{
		return true;
	}

	return (levelData[gridPosition.y][gridPosition.x] != "."); // Will return true when not a dot
}

