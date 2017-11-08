#pragma once

#include <LKengine\audioEngine.h>
#include <LKengine\camera2D.h>
#include <LKengine\debugRenderer.h>
#include <LKengine\inputManager.h>
#include <LKengine\shaderProgram.h>
#include <LKengine\spriteBatch.h>
#include <LKengine\spriteFont.h>
#include <LKengine\particleEngine2D.h>
#include <LKengine\particleBatch2D.h>
#include <LKengine\window.h>


#include "bullet.h"
#include "crosshair.h"
#include "level.h"
#include "player.h"
#include "waves.h"

const float DEPTH_FLOOR = 0.0f;
const float DEPTH_WALL = -2.0f;
const float DEPTH_WALL_DECALS = -2.1f;
const float DEPTH_HUD = -10.0f;

class Zombies;

enum class GameState {
	PLAY,
	EXIT
};


class MainGame
{
public:
    MainGame();
    ~MainGame();

    /// Runs the game
    void run();

private:
    /// Initializes the core systems
    void initSystems();
	/// initialises the level and sets up everything
	void initLevel();
    /// Initializes the shaders
    void initShaders();

    /// Main game loop for the program
    void gameLoop();

	/// updates all agents
	void updateAgents(float deltaTime);
	/// Updates all bullets
	void updateBullets(float deltaTime);

	/// Checks the victor condition
	void checkVictory();

    /// Handles input processing
    void processInput();

    /// Renders the game
    void drawGame();
	/// Draws the HUD
	void drawHUD();


	/// Adds blood to the particle engine
	void addBlood(const glm::vec2& position, int numParticles);
	/// Adds decals to the particle engine
	void addDecal(const glm::vec2& position, int numDecals, float angle, float size);


private:
    /// Member Variables
	LKengine::Window m_window; ///< The game window
	LKengine::ShaderProgram m_textureProgram; ///< The shader program
	LKengine::InputManager m_inputManager; ///< Handles input
	LKengine::DebugRenderer m_debugRenderer;

	LKengine::Camera2D m_camera; ///< Main Camera
	LKengine::Camera2D m_hudCamera; ///< HUD Camera

	LKengine::SpriteBatch m_agentSpriteBatch; ///< Draws all agents
	LKengine::SpriteBatch m_hudSpriteBatch; ///< Draws the HUD
	LKengine::SpriteBatch m_decalSpriteBatch; ///< Draws the decals

	LKengine::SpriteFont* m_spriteFont;

	LKengine::ParticleEngine2D m_particleEngine;
	LKengine::ParticleBatch2D* m_bloodParticleBatch;
	LKengine::ParticleBatch2D* m_decalParticleBatch;


	LKengine::AudioEngine m_audioEngine;

	bool m_renderDebug = true;

	int m_screenWidth = 1280;
	int m_screenHeight = 720;

	float m_fps;
	int m_currentLevel;

	int m_numHumansKilled; ///< Humans lilled by the player
	int m_numZombiesKilled; ///< Zombies lilled by the player

	Player* m_player;


	std::vector<Level*> m_levels; ///< vector of all levels
	//std::vector<Waves*> m_waves; ///< vector of all levels waves
	Waves* m_waves;
	bool generateWave = false;


	//std::vector<Human*> m_humans; ///< Vector of all humans
	std::vector<Zombie*> m_zombies; ///< Vector of all zombies
	std::vector<Bullet> m_bullets; ///< Vector of all bullets
	Crosshair* m_crosshair;

	LKengine::TileSheet m_decalTexture;
	LKengine::TileSheet m_hudTexture;


	GameState m_gameState;



};

