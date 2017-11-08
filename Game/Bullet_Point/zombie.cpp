#include "zombie.h"

#include <LKengine\resourceManager.h>

//#include "human.h"



Zombie::Zombie()
{
}


Zombie::~Zombie()
{
}


void Zombie::init(float speed, glm::vec2 pos)
{
	m_health = 100;
	m_damage = 20;
	
	m_color = LKengine::ColorRGBA8(255, 255, 255, 255);
	m_speed = speed;
	m_position = pos;
	m_textureID = LKengine::ResourceManager::getTexture("Textures/zombie.png").id;

}


void Zombie::update(
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


void Zombie::drawDebug(LKengine::DebugRenderer& debugRenderer)
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


Player* Zombie::getNearestHuman(Player& player)
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