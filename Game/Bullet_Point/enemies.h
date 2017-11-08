#pragma once

#include <glm\glm.hpp>
#include <LKengine\spriteBatch.h>
#include <LKengine\tileSheet.h>
#include <LKengine\debugRenderer.h>

#include "player.h"

const float ENEMY_WIDTH = 45.0f; // 60 original
const float ENEMY_RADIUS = ENEMY_WIDTH / 2.0f;

class Zombie;

class Enemies
{
public:
	Enemies();
	~Enemies();

	void update(
		const std::vector< std::vector<std::string> >& levelData,
		Player& player,
		std::vector<Zombie*>& zombies,
		float deltaTime);

	bool collideWithLevel(const std::vector< std::vector<std::string> >& levelData);

	bool collideWithPlayer(Player* agent);
	bool collideWithEnemy(Enemies* agent);

	void draw(LKengine::SpriteBatch& spriteBatch);

	// Draws a debug circle around the agent the size of their collision detection
	void drawDebug(LKengine::DebugRenderer& debugRenderer);

	// Return true if we died
	bool applyDamage(float damage);

	glm::vec2 getPosition() const { return m_position; }

protected:

	Player* getNearestHuman(Player& player);

	void checkTilePosition(
		const std::vector< std::vector<std::string> >& levelData,
		std::vector<glm::vec2>& collideTilePosition,
		float x, float y);

	void collideWithTile(glm::vec2 tilePos);

	LKengine::TileSheet m_texture;
	LKengine::TileSheet m_textureLegs;
	LKengine::TileSheet m_texture3;
	LKengine::TileSheet m_texture4;
	LKengine::TileSheet m_texture5;
	LKengine::TileSheet m_textureSmoke;

	LKengine::ColorRGBA8 m_color;

	glm::vec2 m_direction = glm::vec2(1.0f, 0.0f);
	glm::vec2 m_position;

	float m_speed;
	float m_health;

	GLuint m_textureID;
};

