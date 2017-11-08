#pragma once


#include <glm\glm.hpp>
#include <vector>

#include <LKengine\spriteBatch.h>

//class Human; /// forward declare
class Enemies; /// forward declare
class Zombie; /// forward declare
//class Agent; /// forward declare

const float BULLET_RADIUS = 2.5;
const float BULLET_SPEED = 5.0f; // USE 20.0f

//class BulletCasing{
//public:
//	BulletCasing(glm::vec2 position, glm::vec2 direction, float speed);
//	~BulletCasing();
//
//	bool update(float deltaTime);
//
//	void draw(LKengine::SpriteBatch& spriteBatch);
//
//};


class Bullet
{
public:
	Bullet(glm::vec2 position, glm::vec2 direction, float damage, float speed);
	~Bullet();

	// When update returns true, delete bullet
	bool update(const std::vector< std::vector<std::string> >& levelData, float deltaTime);

	void draw(LKengine::SpriteBatch& spriteBatch);

	bool collideWithEnemies(Enemies* agent);

	float collisionSide(const std::vector< std::vector<std::string> >& levelData);

	// Getters
	float getDamage() const { return m_damage; }
	float getBulletSize() const { return BULLET_RADIUS;  }
	glm::vec2 getPosition() const { return m_position; }



private:

	bool collideWithWorld(const std::vector< std::vector<std::string> >& levelData);

	float m_damage;
	float m_speed;

	glm::vec2 m_position;
	glm::vec2 m_direction;

	bool m_lrHit = false;
	bool m_tbHit = false;

	//std::vector<BulletCasing> m_bulletCasings;
};

