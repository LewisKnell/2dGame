#pragma once

#include "enemies.h"

const float ZOMBIE_SPEED = 0.3f; // 1.3f original

class Zombie : public Enemies
{
public:
	Zombie();
	~Zombie();

	void init(float speed, glm::vec2 pos);

	void update(
		const std::vector< std::vector<std::string> >& levelData,
		Player& player,
		std::vector<Zombie*>& zombies,
		float deltaTime);

	// Draws a debug circle around the zombie the size of their collision detection
	void drawDebug(LKengine::DebugRenderer& debugRenderer);

	// Getters
	int getDamage() { return m_damage; }


private:
	Player* getNearestHuman(Player& player);

	//Human* getNearestHuman(std::vector<Human*>& humans);

	int m_damage = 0;

};

