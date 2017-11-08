#pragma once

#include <glm\glm.hpp>

#include <LKengine\spriteBatch.h>
#include <LKengine\tileSheet.h>
#include <LKengine\debugRenderer.h>

#include <string>
#include <vector>
#include <ctime>


const float GRENADE_SPEED = 5.0f;
const float GRENADE_WIDTH = 40.0f;

class Grenade
{
public:
	Grenade(std::string name,
		glm::vec2& position,
		const glm::vec2& direction,
		const glm::vec2& destination,
		LKengine::TileSheet texture,
		int tileID);

	~Grenade();

	bool update(const std::vector< std::vector<std::string> >& levelData, float deltaTime);

	void draw(LKengine::SpriteBatch& spriteBatch);

	void drawDebug(LKengine::DebugRenderer& debugRenderer);

	// Getters
	glm::vec2 getPosition() { return m_position; }
	float getTimer() { return m_timer; }
	double getTimePassed() { return m_secondsPassed; }
	

	// Setters
	void setTimer(float timer) { m_timer = timer; }

private:

	bool collideWithWorld(const std::vector< std::vector<std::string> >& levelData);


	std::string m_name;
	glm::vec2 m_position;
	glm::vec2 m_direction;
	glm::vec2 m_destination;
	LKengine::TileSheet m_grenadeTexture;
	int m_tileID;
	clock_t m_startTime;
	double m_secondsPassed = 0;

	float m_timer = 0;

};

