#include "enemies.h"
#include "level.h"

#include <LKengine\resourceManager.h>
#include <algorithm>

#include <iostream>

Enemies::Enemies()
{
}


Enemies::~Enemies()
{
}


void Enemies::update(
	const std::vector< std::vector<std::string> >& levelData,
	Player& player,
	std::vector<Zombie*>& zombies,
	float deltaTime)
{

	Player* closestHuman = getNearestHuman(player);

	//make zombies chase humans
	if (closestHuman != nullptr)
	{
	m_direction = glm::normalize(player.getPlayerPosition() - m_position);
	m_position += m_direction * m_speed * deltaTime;
	}

	collideWithLevel(levelData);

}


bool Enemies::collideWithLevel(const std::vector< std::vector<std::string> >& levelData)
{
	std::vector<glm::vec2> collideTilePosition;

	//check the 4 corners
	//first corner
	checkTilePosition(levelData, collideTilePosition,
		m_position.x, m_position.y);
	//second corner
	checkTilePosition(levelData, collideTilePosition,
		m_position.x + ENEMY_WIDTH, m_position.y);
	//third corner
	checkTilePosition(levelData, collideTilePosition,
		m_position.x, m_position.y + ENEMY_WIDTH);
	//fourth corner
	checkTilePosition(levelData, collideTilePosition,
		m_position.x + ENEMY_WIDTH, m_position.y + ENEMY_WIDTH);

	if (collideTilePosition.size() == 0) {
		return false;
	}

	//do the collision
	for (int i = 0; i < collideTilePosition.size(); i++)
	{
		collideWithTile(collideTilePosition[i]);
	}
	return true;
}


bool Enemies::collideWithPlayer(Player* agent)
{
	const float MIN_DISTANCE = ENEMY_RADIUS + PLAYER_RADIUS;

	glm::vec2 centrePosA = m_position + glm::vec2(ENEMY_RADIUS);
	glm::vec2 centrePosB = agent->getPlayerPosition() + glm::vec2(PLAYER_RADIUS);

	glm::vec2 distVec = centrePosA - centrePosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;
	if (collisionDepth > 0)
	{
		glm::vec2 collisionDepthVec = glm::normalize(distVec) * collisionDepth;

		m_position += collisionDepthVec / 2.0f;
		//agent->m_position -= collisionDepthVec / 2.0f;
		agent->setPlayerPosition(-(collisionDepthVec / 2.0f));
		return true;
	}
	return false;

}


bool Enemies::collideWithEnemy(Enemies* enemy)
{
	const float MIN_DISTANCE = ENEMY_RADIUS * 2;

	glm::vec2 centrePosA = m_position + glm::vec2(ENEMY_RADIUS);
	glm::vec2 centrePosB = enemy->getPosition() + glm::vec2(ENEMY_RADIUS);

	glm::vec2 distVec = centrePosA - centrePosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;
	if (collisionDepth > 0)
	{
		glm::vec2 collisionDepthVec = glm::normalize(distVec) * collisionDepth;

		m_position += collisionDepthVec / 2.0f;
		enemy->m_position -= collisionDepthVec / 2.0f;
		return true;
	}
	return false;

}


void Enemies::draw(LKengine::SpriteBatch& spriteBatch)
{
	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

	glm::vec4 destRect;
	destRect.x = m_position.x;
	destRect.y = m_position.y;
	destRect.z = ENEMY_WIDTH;
	destRect.w = ENEMY_WIDTH;

	spriteBatch.draw(destRect, uvRect, m_textureID, 0.0f, m_color, m_direction);
}


void Enemies::drawDebug(LKengine::DebugRenderer& debugRenderer)
{
	LKengine::ColorRGBA8 color(255, 255, 255, 255);

	glm::vec4 destRect;
	destRect.x = m_position.x;
	destRect.y = m_position.y;
	destRect.z = ENEMY_WIDTH;
	destRect.w = ENEMY_WIDTH;

	// Draw circle
	debugRenderer.drawCircle(glm::vec2(destRect.x + ENEMY_RADIUS, destRect.y + ENEMY_RADIUS), color, ENEMY_RADIUS);

}


bool Enemies::applyDamage(float damage)
{
	m_health -= damage;

	if (m_health <= 0)
	{
		return true;
	}
	return false;
}


void Enemies::checkTilePosition(
	const std::vector< std::vector<std::string> >& levelData,
	std::vector<glm::vec2>& collideTilePosition,
	float x, float y)
{

	glm::vec2 cornerPos = glm::vec2(floor(x / (float)TILE_WIDTH),
		floor(y / (float)TILE_WIDTH));

	//if outside world, return
	if (cornerPos.x < 0 || cornerPos.x >= levelData[0].size() ||
		cornerPos.y < 0 || cornerPos.y >= levelData.size())
	{
		return;
	}

	if (levelData[cornerPos.y][cornerPos.x] != ".")
	{
		collideTilePosition.push_back(cornerPos * (float)TILE_WIDTH + glm::vec2((float)TILE_WIDTH / 2));
	}

}

//AABB colision
void Enemies::collideWithTile(glm::vec2 tilePos)
{

	const float TILE_RADIUS = (float)TILE_WIDTH / 2.0f;
	// The minimum distance before a collision occurs
	const float MIN_DISTANCE = ENEMY_RADIUS + TILE_RADIUS;

	// Centre position of the agent
	glm::vec2 centrePlayerPos = m_position + glm::vec2(ENEMY_RADIUS);
	// Vector from the agent to the tile
	glm::vec2 distVec = centrePlayerPos - tilePos;

	// Get the depth of the collision
	float xDepth = MIN_DISTANCE - abs(distVec.x);
	float yDepth = MIN_DISTANCE - abs(distVec.y);

	//if either of the depths are > 0, then we collided
	if (xDepth > 0 && yDepth > 0)
	{
		// Check which collision depth is less
		if (std::max(xDepth, 0.0f) < std::max(yDepth, 0.0f))
		{
			// X collision depth is smaller so we push in the X direction
			if (distVec.x < 0) {
				m_position.x -= xDepth;
			}
			else {
				m_position.x += xDepth;
			}
		}
		else
		{
			// Y collision depth is smaller so we push in the Y direction
			if (distVec.y < 0) {
				m_position.y -= yDepth;
			}
			else {
				m_position.y += yDepth;
			}
		}
	}
}


Player* Enemies::getNearestHuman(Player& player)
{
	Player* closestHuman = nullptr; //return null pointer if no humans
	float smallestDistance = 9999999.0f;

	//for (int i = 0; i < humans.size(); i++)
	//{
		glm::vec2 distVec = player.getPlayerPosition() - m_position;
		float distance = glm::length(distVec);

		//make current human the closest human if distance is smaller
		//than the currently held nearest human
		if (distance < smallestDistance)
		{
			smallestDistance = distance;
			closestHuman = &player;
		}
	//}

	return closestHuman;
}