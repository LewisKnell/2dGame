#pragma once

#include <glm\glm.hpp>
#include <LKengine\spriteBatch.h>
#include <LKengine\inputManager.h>
#include <LKengine\Camera2D.h>
#include <LKengine\tileSheet.h>
#include <LKengine\debugRenderer.h>

#include "zombie.h"

const float DEPTH_CROSSHAIR = -8.0f;
const float CROSSHAIR_WIDTH = 35.0f;
const float CROSSHAIR_RADIUS = CROSSHAIR_WIDTH / 2;


class Crosshair
{
public:
	Crosshair();
	~Crosshair();

	void init(LKengine::InputManager* inputManager, LKengine::Camera2D* camera);

	void draw(LKengine::SpriteBatch& spriteBatch, std::vector<Zombie*> zombies);

	// Draws a debug circle around the crosshair the size of its collision detection
	void drawDebug(LKengine::DebugRenderer& debugRenderer);


	bool overEnemy(Enemies* agent);

private:
	LKengine::InputManager* m_inputManager;
	LKengine::Camera2D* m_camera;

	LKengine::ColorRGBA8 m_color;

	LKengine::TileSheet m_textureCrosshair;
	int m_tileIndexCrosshair = 0;

	//std::vector<Zombie*> m_zombies; ///< Vector of all zombies

	glm::vec4 m_destRectCrosshair;
};

