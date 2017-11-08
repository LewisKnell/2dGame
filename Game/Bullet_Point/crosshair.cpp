#include "crosshair.h"

#include <LKengine\resourceManager.h>

#include <iostream>


Crosshair::Crosshair()
{
}


Crosshair::~Crosshair()
{
}


void Crosshair::init(LKengine::InputManager* inputManager, LKengine::Camera2D* camera)
{

	m_inputManager = inputManager;
	m_camera = camera;

	m_color = LKengine::ColorRGBA8(255, 255, 255, 255);


	LKengine::GLTexture textureCrosshair = LKengine::ResourceManager::getTexture("Textures/Misc/crosshair.png");

	m_textureCrosshair.init(textureCrosshair, glm::ivec2(2, 2));
}


void Crosshair::draw(LKengine::SpriteBatch& spriteBatch, std::vector<Zombie*> zombies)
{
	glm::vec2 mouseCoords = m_inputManager->getMouseCoords();
	mouseCoords = m_camera->convertScreenToWorld(mouseCoords);

	m_destRectCrosshair.x = mouseCoords.x - CROSSHAIR_RADIUS;
	m_destRectCrosshair.y = mouseCoords.y - CROSSHAIR_RADIUS;
	m_destRectCrosshair.z = CROSSHAIR_WIDTH;
	m_destRectCrosshair.w = CROSSHAIR_WIDTH;

	for (int i = 0; i < zombies.size(); i++)
	{
		if (overEnemy(zombies[i]) == true)
		{
			// Red crosshair if over enemy
			m_tileIndexCrosshair = 3;
			break;
		}
		else
		{
			// Green crosshair if not over enemy
			m_tileIndexCrosshair = 2;
		}
	}

	// Get the uv coordinates from the tile index
	glm::vec4 uvRectCrosshair = m_textureCrosshair.getUVs(m_tileIndexCrosshair);

	// Draw the crosshair sprite
	spriteBatch.draw(m_destRectCrosshair, uvRectCrosshair, m_textureCrosshair.texture.id, DEPTH_CROSSHAIR, m_color, glm::vec2(1, 0));
}


void Crosshair::drawDebug(LKengine::DebugRenderer& debugRenderer)
{
	LKengine::ColorRGBA8 color(255, 255, 255, 255);

	glm::vec4 destRect;
	destRect.x = m_destRectCrosshair.x;
	destRect.y = m_destRectCrosshair.y;
	destRect.z = CROSSHAIR_WIDTH;
	destRect.w = CROSSHAIR_WIDTH;

	// Draw circle
	debugRenderer.drawCircle(glm::vec2(destRect.x + CROSSHAIR_RADIUS, destRect.y + CROSSHAIR_RADIUS), color, CROSSHAIR_RADIUS);

}


bool Crosshair::overEnemy(Enemies* agent)
{
	const float MIN_DISTANCE = ENEMY_RADIUS;

	glm::vec2 centrePosA = m_destRectCrosshair + CROSSHAIR_RADIUS;
	glm::vec2 centrePosB = agent->getPosition() + glm::vec2(ENEMY_RADIUS);

	glm::vec2 distVec = centrePosA - centrePosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;
	if (collisionDepth > 0)
	{
		return true;
	}
	return false;

}