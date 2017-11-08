#include "MainGame.h"

#include <SDL/SDL.h>
#include <random>
#include <ctime>
#include <iostream>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\rotate_vector.hpp>

#include <LKengine\LKengine.h>
#include <LKengine\timing.h>
#include <LKengine\myErrors.h>
#include <LKengine\resourceManager.h>

#include "gun.h"
#include "zombie.h"


//const float ZOMBIE_SPEED = 1.3f;



MainGame::MainGame() :
	m_screenWidth(1280),
	m_screenHeight(720),
	m_fps(0),
	m_gameState(GameState::PLAY),
	m_player(nullptr),
	m_numZombiesKilled(0)
{
	// Empty
}


MainGame::~MainGame()
{
	// Deletes the levels
	for (int i = 0; i < m_levels.size(); i++)
	{
		delete m_levels[i];
	}
	// Deletes zombies
	for (int i = 0; i < m_zombies.size(); i++)
	{
		delete m_zombies[i];
	}
}


void MainGame::run()
{
	initSystems();

	initLevel();

	LKengine::Music music = m_audioEngine.loadMusic("Sounds/Music/XYZ.ogg");
	music.play(); // Play music once

	gameLoop();

	//system("PAUSE");
}


void MainGame::initSystems()
{
	// Initialise the game engine
	LKengine::init();

	// Initialise sound (must happen after LKengine::init())
	m_audioEngine.init();

	// Create the SDL window
	m_window.createWindow("Bullet Point", m_screenWidth, m_screenHeight, 0);
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	// Hide the mouse cursor - Disable when mouse cursor is replaced with crosshair texture at mouse coords
	SDL_ShowCursor(SDL_DISABLE);

	// Initialise the shaders
	initShaders();

	// Initialise the Debug Renderer
	m_debugRenderer.init();

	// Initialise the Agent sprites
	m_agentSpriteBatch.init();
	// Initialise the HUD sprites
	m_hudSpriteBatch.init();
	// Initialise the Decal sprites
	m_decalSpriteBatch.init();

	// Initialise the sprite font (after initilise SDL) 
	//and OpenGL otherwise will get errors) 
	m_spriteFont = new LKengine::SpriteFont("Fonts/texgyreadventor-bold.otf", 64);


	// Initialise the cameras
	m_camera.init(m_screenWidth, m_screenHeight);
	m_hudCamera.init(m_screenWidth, m_screenHeight);
	// Offset the HUD camera to start in bottom left
	m_hudCamera.setPosition(glm::vec2(m_screenWidth / 2, m_screenHeight / 2));


	LKengine::GLTexture texture;

	// Initialise particles
	// Load blood sprite sheet
	texture = LKengine::ResourceManager::getTexture("Textures/Misc/smoke_particle.png");
	// Set the size of decal sprite sheet
	m_decalTexture.init(texture, glm::ivec2(1, 1));

	m_bloodParticleBatch = new LKengine::ParticleBatch2D();
	m_bloodParticleBatch->init(1000, 0.05f,
		m_decalTexture,
		0,
		[](LKengine::Particle2D& particle, float deltaTime)
	{
		particle.position += particle.velocity * deltaTime;
		particle.color.a = (GLubyte)(particle.life * 255.0f);
	});

	m_particleEngine.addParticleBatch(m_bloodParticleBatch);


	// Initialise the decals
	// Load decals sprite sheet
	texture = LKengine::ResourceManager::getTexture("Textures/Misc/decals.png");
	// Set the size of decal sprite sheet
	m_decalTexture.init(texture, glm::ivec2(18, 4));

	// Initialise decal (particles)
	m_decalParticleBatch = new LKengine::ParticleBatch2D();
	m_decalParticleBatch->init(1000, 0.0f,
		m_decalTexture,
		18,
		[](LKengine::Particle2D& particle, float deltaTime)
	{
		particle.position += particle.velocity * deltaTime;
		particle.color.a = (GLubyte)(particle.life * 255.0f);
	});

	m_particleEngine.addParticleBatch(m_decalParticleBatch);


	// Initialise the textures used for HUD elements
	LKengine::GLTexture textureHUD = LKengine::ResourceManager::getTexture("Textures/Misc/hud_symbols.png");
	m_hudTexture.init(textureHUD, glm::ivec2(13, 1));


	// Initialise the crosshair
	m_crosshair = new Crosshair();
	m_crosshair->init(&m_inputManager, &m_camera);


}


void MainGame::initLevel()
{
	// Initialise the first level (level 1)
	m_levels.push_back(new Level("Levels/level1.txt"));
	m_currentLevel = 0;


	// Load and initialise the levels enemy waves data
	m_waves = new Waves();
	m_waves->init("Waves/level1Waves.txt");
	// Generate the first wave
	m_waves->generateWave(m_levels[m_currentLevel]->getZombieStartPos(), m_zombies);


	m_player = new Player();
	m_player->init(m_levels[m_currentLevel]->getStartPlayerPos(), &m_inputManager, &m_camera, &m_bullets);


//	std::mt19937 randomEngine;
//	randomEngine.seed(time(nullptr));



	//Set up the players guns
	m_player->initGuns();

}


void MainGame::initShaders() {
	// Compile our color shader
	m_textureProgram.compileShaders("Shaders/vertexShader.vs", "Shaders/fragmentShader.fs");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColor");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();
}


void MainGame::gameLoop()
{
	// Some helpful constants
	const float DESIRED_FPS = 60; // FPS the game is designed to run at
	const int MAX_PHYSICS_STEPS = 5; // Max number of physics steps per frame
	const float MS_PER_SECOND = 1000; // Number of milliseconds in a second
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS; // The desired frame time per frame
	const float MAX_DELTA_TIME = 0.25f; // Maximum size of deltaTime

	// Used to cap the FPS
	LKengine::FPSLimiter m_fpsLimiter;
	m_fpsLimiter.setMaxFPS(60.0f);

	// Zoom out the camera by 1.5x ***zoom level*** (use 1.5 for final game)
	const float CAMERA_SCALE = 1.0f / 1.0f;
	m_camera.setScale(CAMERA_SCALE);

	// Start the previous ticks variable
	float previousTicks = SDL_GetTicks();

	// Main loop
	while (m_gameState == GameState::PLAY)
	{
		m_fpsLimiter.beginFrame();

		// Calculate the frameTime in milliseconds
		Uint32 newTicks = SDL_GetTicks();
		Uint32 frameTime = newTicks - previousTicks;
		previousTicks = newTicks; // Store newTicks in previousTicks so we can use it next frame
		// Get the total delta time
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		// Check if victory conditions have been met
		checkVictory();

		m_inputManager.update();

		processInput();

		int i = 0; // This counter makes sure we don't spiral to death!
		// Loop while we still have steps to process
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS)
		{
			// The deltaTime should be the smaller of the totalDeltaTime and MAX_DELTA_TIME
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			// Update all physics here and pass in deltaTime
			updateAgents(deltaTime);

			m_particleEngine.update(deltaTime);
			// Since we just took a step that is length deltaTime, subtract from totalDeltaTime
			totalDeltaTime -= deltaTime;
			// Increment our frame counter so we can limit steps to MAX_PHYSICS_STEPS
			i++;
		}


		// Have more update frames for faster moving objects (eg bullets)
		i = 0;
		totalDeltaTime = frameTime / DESIRED_FRAMETIME;
		while (i < MAX_PHYSICS_STEPS*2)
		{
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			updateBullets(deltaTime);
			totalDeltaTime -= deltaTime;
			i++;
		}

		// Make sure the camera is bound to the player position
		m_camera.setPosition(m_player->getPlayerPosition());
		m_camera.update();
		// Update the HUD camera
		m_hudCamera.update();

		drawGame();

		// End the frame, limit the FPS and get the current FPS
		m_fps = m_fpsLimiter.end();
		// Print out the FPS to console
		//std::cout << m_fps << std::endl;
	}
}


void MainGame::updateAgents(float deltaTime)
{
	//update all zombies
	for (int i = 0; i < m_zombies.size(); i++)
	{
		m_zombies[i]->update(m_levels[m_currentLevel]->getLevelData(), *m_player, m_zombies, deltaTime);
	}


	//update Zombie collisions
	for (int i = 0; i < m_zombies.size(); i++)
	{
		//collide with other zombies
		for (int j = i + 1; j < m_zombies.size(); j++)
		{
			m_zombies[i]->collideWithEnemy(m_zombies[j]);
		}

		//zombies collide with player
		if (m_zombies[i]->collideWithPlayer(m_player))
		{
			m_player->applyDamage(m_zombies[i]->getDamage());


			std::cout << "armour " << m_player->getPlayerArmour() << std::endl;
			std::cout << "health " << m_player->getPlayerHealth() << std::endl;
			
		}
	}

	m_player->update(m_levels[m_currentLevel]->getLevelData(), deltaTime);

}


void MainGame::updateBullets(float deltaTime)
{
	// Update and collide with world
	for (int i = 0; i < m_bullets.size(); )
	{
		// If update returns true, the bullet collided with a wall
		if (m_bullets[i].update(m_levels[m_currentLevel]->getLevelData(), deltaTime))
		{
			// Add decal when wall hit
			float collideSide = m_bullets[i].collisionSide(m_levels[m_currentLevel]->getLevelData());

			float bulletImpactDecalSize = 30;
			glm::vec2 adjustmentVec;
			adjustmentVec.x = 0;
			adjustmentVec.y = 0;

			std::cout << "collideSide: " << collideSide << std::endl;
			if (collideSide == 90) // Right side
			{
				std::cout << "Right side hit" << std::endl;
				adjustmentVec.x = -bulletImpactDecalSize;
				adjustmentVec.y = -bulletImpactDecalSize / 2;
			}
			else if (collideSide == 270) // Left side
			{
				std::cout << "Left side hit" << std::endl;
				adjustmentVec.y = -bulletImpactDecalSize / 2;
			}
			else if (collideSide == 180) // Top side
			{
				std::cout << "Top side hit" << std::endl;
				adjustmentVec.y = -bulletImpactDecalSize;
				adjustmentVec.x = -bulletImpactDecalSize / 2;
			}
			else if (collideSide == 0) // Bottom side
			{
				std::cout << "Bottom side hit" << std::endl;
				adjustmentVec.x = -bulletImpactDecalSize / 2;
			}

			// Add the decal where the bullet impacted, adjusted position for decal to be centralised on impact point
			addDecal(m_bullets[i].getPosition() + adjustmentVec, 1, collideSide, bulletImpactDecalSize);

			m_bullets[i] = m_bullets.back();
			m_bullets.pop_back();
		}
		// Dont increment i++ so we update the bullet we swap in
		else { i++; }
	}

	bool wasBuletRemoved;

	// Collide with zombies
	for (int i = 0; i < m_bullets.size(); i++)
	{
		wasBuletRemoved = false;
		// Loop through zombies
		for (int j = 0; j < m_zombies.size(); )
		{
			// Check bullet collision with zombies
			if (m_bullets[i].collideWithEnemies(m_zombies[j]))
			{
				// Add blood
				addBlood(m_bullets[i].getPosition(), 5);

				// Damage zombie and kill it if it's out of health
				if (m_zombies[j]->applyDamage(m_bullets[i].getDamage()))
				{
					// If the zombie died, remove him
					delete m_zombies[j];
					m_zombies[j] = m_zombies.back();
					m_zombies.pop_back();
					// Add to number of zombies killed for statistics
					m_numZombiesKilled++;
					// Add money for killing enemy
					m_player->addPlayerMoney(50);
				}
				else { j++; }

				// Remove the bullet
				m_bullets[i] = m_bullets.back();
				m_bullets.pop_back();
				wasBuletRemoved = true;
				i--; // Make sure we don't skip a bullet
				// Since the bullet died, no need to loop through any more zombies
				break;
			}
			else { j++; }
		}
	}
}


void MainGame::checkVictory()
{
	//TODO: Support for multiple levels
	// _currentLevel++; initLevel(...)

	//std::cout << "Current wave number: " << m_waves->getCurrentWaveNumber() << std::endl;
	//std::cout << "Number of waves: " << m_waves->getNumWaves() << std::endl;

	// Check if all zombies are dead. If so, increase the wave number
	if (m_zombies.empty())
	{
		m_waves->increaseWaveNumber();
		generateWave = true;

	}

	// If new wave is needed and wave number is <= the number of waves in the level, then generate the new waves enemies
	if (generateWave && m_waves->getCurrentWaveNumber() <= m_waves->getNumWaves())
	{
		m_waves->generateWave(m_levels[m_currentLevel]->getZombieStartPos(), m_zombies);
		generateWave = false;
	}
	// If all zombies are dead and no more waves are left then we win!
	if (m_waves->getCurrentWaveNumber() > m_waves->getNumWaves())
	{ 
		std::printf("*** You Win! ***\nYou killed %d zombies.\n", m_numZombiesKilled);
			LKengine::fatalError("You Win!");
	}
	// If player has lost all health, then we lose!
	if (m_player->getPlayerHealth() <= 0)
	{
		std::printf("*** You Win! ***\nYou killedand %d zombies.\n", m_numZombiesKilled);
		LKengine::fatalError("You Win!");
	}


}


void MainGame::processInput() 
{
	SDL_Event evnt;

	// Will keep looping until there are no more events to process
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
		case SDL_QUIT:
			m_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			m_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
			break;
		case SDL_KEYDOWN:
			m_inputManager.pressKey(evnt.key.keysym.sym);
			break;
		case SDL_KEYUP:
			m_inputManager.releaseKey(evnt.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			m_inputManager.pressKey(evnt.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			m_inputManager.releaseKey(evnt.button.button);
			break;
		}
	}


	// Volume up (all volumes)
	if (m_inputManager.isKeyPressed(SDLK_PAGEUP))
	{
		m_audioEngine.volumeUp(10);
	}
	// Volume down (all volumes)
	if (m_inputManager.isKeyPressed(SDLK_PAGEDOWN))
	{
		m_audioEngine.volumeDown(10);
	}

}


void MainGame::drawGame() 
{
	// Set the base depth to 1.0
	glClearDepth(1.0);
	// Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_textureProgram.use();

	// Draw code goes here
	glActiveTexture(GL_TEXTURE0);

	// Make sure the shader uses texture 0
	GLint textureUniform = m_textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	// Grab the camera matrix
	glm::mat4 projectionMatrix = m_camera.getCameraMatrix();
	GLint transUniform = m_textureProgram.getUniformLocation("transformationMatrix");
	glUniformMatrix4fv(transUniform, 1, GL_FALSE, &projectionMatrix[0][0]);


	// Draw the level
	m_levels[m_currentLevel]->draw();

	// Begin drawing agents
	m_agentSpriteBatch.begin(LKengine::GlyphSortType::BACK_TO_FRONT);

	// Set agent dimensions for camera culling. This needs to change if have diff size agents
	const glm::vec2 agentDimensions(ENEMY_RADIUS * 2.0f);



	// Draw the player
	m_player->draw(m_agentSpriteBatch);

	// Draw the crosshair
	m_crosshair->draw(m_agentSpriteBatch, m_zombies);

	// Draw the zombies
	for (int i = 0; i < m_zombies.size(); i++) {
		if (m_camera.isBoxInView(m_zombies[i]->getPosition(), agentDimensions)) {
			m_zombies[i]->draw(m_agentSpriteBatch);
		}
	}

	// Draw the bullets
	for (int i = 0; i < m_bullets.size(); i++) {
		m_bullets[i].draw(m_agentSpriteBatch);
	}

	// End spritebatch creation
	m_agentSpriteBatch.end();
	// Render to the screen
	m_agentSpriteBatch.renderBatch();


	//begin drawing the particles
	m_agentSpriteBatch.begin(LKengine::GlyphSortType::BACK_TO_FRONT);
	/// Render the particles. Use agentSpriteBatch so we don't need to make
	/// a new sprite batch and so we use the same memory buffer.
	m_particleEngine.draw(&m_agentSpriteBatch);
	// End spritebatch creation
	m_agentSpriteBatch.end();
	// render to the screen
	m_agentSpriteBatch.renderBatch();


	// Render the HUD
	drawHUD();


	// Debug Rendering
	if (m_renderDebug)
	{
		// Render player debug 
		m_player->drawDebug(m_debugRenderer);
		// Render the crosshair debug
		m_crosshair->drawDebug(m_debugRenderer);

		// Render the level debug
		m_levels[m_currentLevel]->drawDebug(m_debugRenderer);
		
		for (int i = 0; i < m_zombies.size(); i++)
		{
			m_zombies[i]->drawDebug(m_debugRenderer);
		}

		m_debugRenderer.end();
		m_debugRenderer.render(projectionMatrix, 2.0f);
	}


	// Unbind the program
	m_textureProgram.unuse();

	//Swap our buffer and draw everything to the screen!
	m_window.swapBuffer();
}


void MainGame::drawHUD()
{
	char buffer[256];

	//grab the camera matrix
	glm::mat4 projectionMatrix = m_hudCamera.getCameraMatrix();
	GLint transUniform = m_textureProgram.getUniformLocation("transformationMatrix");
	glUniformMatrix4fv(transUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	LKengine::ColorRGBA8 colorHUD = LKengine::ColorRGBA8(255, 255, 255, 255);
	int tileIndex = 0;
	int textOffset = 0;

	m_hudSpriteBatch.begin(LKengine::GlyphSortType::BACK_TO_FRONT);

	// Bottom Left Area
	glm::vec4 destRectHealth;
	destRectHealth.x = 10;
	destRectHealth.y = 10;
	destRectHealth.z = 30;
	destRectHealth.w = 30;
	textOffset = 10;
	tileIndex = 0;
	glm::vec4 uvRectHealth = m_hudTexture.getUVs(tileIndex);
	// Draw the player health icon (bottom left)
	m_hudSpriteBatch.draw(destRectHealth, uvRectHealth, m_hudTexture.texture.id, DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), glm::vec2(1, 0));
	// Draw the players health number (bottom left)
	sprintf_s(buffer, "%d", m_player->getPlayerHealth());
	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(destRectHealth.x + destRectHealth.z + textOffset, destRectHealth.y - 10),
		glm::vec2(0.5), DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), LKengine::Justification::LEFT);
	
	glm::vec4 destRectArmour;
	destRectArmour.x = destRectHealth.x + destRectHealth.z + 150;
	destRectArmour.y = 10;
	destRectArmour.z = 30;
	destRectArmour.w = 30;
	textOffset = 10;
	tileIndex = 1;
	glm::vec4 uvRectArmour = m_hudTexture.getUVs(tileIndex);
	// Draw the player health icon (bottom left)
	m_hudSpriteBatch.draw(destRectArmour, uvRectArmour, m_hudTexture.texture.id, DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), glm::vec2(1, 0));
	// Draw the players armour number (bottom left)
	sprintf_s(buffer, "%d", m_player->getPlayerArmour());
	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(destRectArmour.x + destRectArmour.z + textOffset, destRectArmour.y - 10),
		glm::vec2(0.5), DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), LKengine::Justification::LEFT);



	// Bottom Right Area

	// Draw ammo left in magazine (bottom right)
	glm::vec4 destRectMagLeft;
	destRectMagLeft.x = 500;
	destRectMagLeft.y = 10;
	destRectMagLeft.z = 30;
	destRectMagLeft.w = 30;
	sprintf_s(buffer, "%d", m_player->getMagLeft());
	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(destRectMagLeft.x, destRectMagLeft.y - 10),
		glm::vec2(0.5), DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), LKengine::Justification::LEFT);

	sprintf_s(buffer, " | " );
	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(destRectMagLeft.x + 45, destRectMagLeft.y - 7.5),
		glm::vec2(0.5), DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), LKengine::Justification::LEFT);

	// Draw ammo left to reload magazines with (bottom right)
	glm::vec4 destRectAmmoTotalLeft;
	destRectAmmoTotalLeft.x = destRectMagLeft.x + 75;
	destRectAmmoTotalLeft.y = destRectMagLeft.y;
	destRectAmmoTotalLeft.z = 30;
	destRectAmmoTotalLeft.w = 30;
	sprintf_s(buffer, "%d", m_player->getAmmoTotalLeft());
	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(destRectAmmoTotalLeft.x, destRectAmmoTotalLeft.y - 10),
		glm::vec2(0.5), DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), LKengine::Justification::LEFT);

	// Draw players money
	glm::vec4 destRectMoney;
	destRectMoney.x = destRectMagLeft.x;
	destRectMoney.y = destRectAmmoTotalLeft.y + 30;
	destRectMoney.z = 30;
	destRectMoney.w = 30;
	textOffset = 50;
	tileIndex = 7;
	glm::vec4 uvRectMoney = m_hudTexture.getUVs(tileIndex);
	// Draw the money icon (bottom left)
	m_hudSpriteBatch.draw(destRectMoney, uvRectMoney, m_hudTexture.texture.id, DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), glm::vec2(1, 0));
	// Draw the players money value
	sprintf_s(buffer, "%d", m_player->getPlayerMoney());
	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(destRectMoney.x + textOffset, destRectMoney.y - 10),
		glm::vec2(0.5), DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), LKengine::Justification::LEFT);



	// Top Left Area
	// Draw enemies left stuff
	glm::vec4 destRectEnemiesLeft;
	destRectEnemiesLeft.x = 10;
	destRectEnemiesLeft.y = 570;
	destRectEnemiesLeft.z = 30;
	destRectEnemiesLeft.w = 30;
	textOffset = 50;
	tileIndex = 12;
	glm::vec4 uvRectEnemiesLeft = m_hudTexture.getUVs(tileIndex);
	// Draw the enemy icon (top left)
	m_hudSpriteBatch.draw(destRectEnemiesLeft, uvRectEnemiesLeft, m_hudTexture.texture.id, DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), glm::vec2(1, 0));
	// Draw the enemies left in wave value
	sprintf_s(buffer, "%d", m_zombies.size());
	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(destRectEnemiesLeft.x + textOffset, destRectEnemiesLeft.y - 10),
		glm::vec2(0.5), DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), LKengine::Justification::LEFT);

	// Draw the words "Enemies Left" (above the number and icon)
	sprintf_s(buffer, "Wave:");
	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(destRectEnemiesLeft.x, destRectEnemiesLeft.y + 30),
		glm::vec2(0.5), DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), LKengine::Justification::LEFT);
	sprintf_s(buffer, "%d / %d", m_waves->getCurrentWaveNumber(), m_waves->getNumWaves());
	m_spriteFont->draw(m_hudSpriteBatch, buffer, glm::vec2(destRectEnemiesLeft.x + 100, destRectEnemiesLeft.y + 30),
		glm::vec2(0.5), DEPTH_HUD, LKengine::ColorRGBA8(255, 255, 255, 255), LKengine::Justification::LEFT);


	m_hudSpriteBatch.end();
	m_hudSpriteBatch.renderBatch();
}


void MainGame::addBlood(const glm::vec2& position, int numParticles)
{
	static std::mt19937 randEngine(time(nullptr));
	static std::uniform_real_distribution<float> randAngle(0.0f, 2.0f * (float)M_PI);

	glm::vec2 vel(2.0f, 0.0f);
	LKengine::ColorRGBA8 col(255, 0, 0, 255);

	for (int i = 0; i < numParticles; i++) {
		m_bloodParticleBatch->addParticle(
			position,
			glm::rotate(vel, randAngle(randEngine)),
			0.0f,
			col,
			20.0f);
	}
}


void MainGame::addDecal(const glm::vec2& position, int numDecals, float angle, float size)
{

	glm::vec2 vel(0.0f, 0.0f);
	LKengine::ColorRGBA8 col(255, 255, 255, 255);

	angle = angle * (float)M_PI / 180;
	for (int i = 0; i < numDecals; i++) {
		m_decalParticleBatch->addParticle(
			position,
			glm::rotate(vel, 0.0f),
			angle,
			col,
			size);
	}
}



