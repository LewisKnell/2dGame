
#include "gun.h"
#include "player.h"

#include <LKengine\resourceManager.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\rotate_vector.hpp>
#include <SDL\SDL.h>
#include <iostream>
#include <random> // For random engine
#include <ctime> // For time used in random engine


Player::Player() :
	m_currentGunIndex(-1)
{
	//Empty
}


Player::~Player()
{
	//Empty
}


void Player::init(glm::vec2 pos, LKengine::InputManager* inputManager, LKengine::Camera2D* camera, std::vector<Bullet>* bullets)
{
	// Initialise player variables
	m_health = 100;
	m_armour = 100;
	m_money = 0;
	m_speed = PLAYER_FORWARD_SPEED;
	m_capacity = 20;
	m_stamina = 10.0f;
	m_numGrenades = 10;

	m_inputManager = inputManager;
	m_camera = camera;
	m_bullets = bullets;
	m_position = pos;
	
	m_color = LKengine::ColorRGBA8(255, 255, 255, 255);
	m_currentGunIndex = -1;

	// Set the gun offset and length for AK being starting weapon
	GUN_OFFSET = 20;
	GUN_LENGTH = 5;

	// Initialise sounds
	initSounds();	
	// declare texture with default circle texture (if this shows, the texture hasn't been set up properly
	LKengine::GLTexture texture = LKengine::ResourceManager::getTexture("Textures/circle.png");;
	// Initialise textures
	initTextures(texture);
	//Initialise particle systems
	initParticleSystems(texture);
	
}


void Player::initGuns()
{
	//Set up the players guns
	const float BULLET_SPEED = 5.0f; // USE 20
	addGun(new Gun("AK47", 9, 1, 4.0f, 20.0f, BULLET_SPEED, 120, 30, RIFLE_WEIGHT, m_audioEngine.loadSoundEffect("Sounds/Shots/ak47.wav")));
	addGun(new Gun("M3", 55, 12, 7.5f, 10.0f, BULLET_SPEED, 40, 8, SHOTGUN_WEIGHT, m_audioEngine.loadSoundEffect("Sounds/Shots/m3.wav")));
	addGun(new Gun("Deagle", 20, 1, 3.0f, 30.0f, BULLET_SPEED, 42, 7, PISTOL_WEIGHT, m_audioEngine.loadSoundEffect("Sounds/Shots/deagle.wav")));
}


void Player::initSounds()
{
	// Gun sound effects
	// Reload sound effects
	m_clipOutSound = m_audioEngine.loadSoundEffect("Sounds/Misc/clipout.wav");
	m_clipInSound = m_audioEngine.loadSoundEffect("Sounds/Misc/clipin.wav");
	//Empty magazine sound effect
	m_emptyMagazine = m_audioEngine.loadSoundEffect("Sounds/Misc/click.wav");

	// Footstep sound effects
	m_footstepGrass1 = m_audioEngine.loadSoundEffect("Sounds/Player/dirt1.wav");
	m_footstepGrass2 = m_audioEngine.loadSoundEffect("Sounds/Player/dirt2.wav");
	m_footstepGrass3 = m_audioEngine.loadSoundEffect("Sounds/Player/dirt3.wav");

	// Misc Sounds
	//Sound for player being hit
	m_playerHurt1 = m_audioEngine.loadSoundEffect("Sounds/Player/player_hit01.wav");
	m_playerHurt2 = m_audioEngine.loadSoundEffect("Sounds/Player/player_hit02.wav");
}


void Player::initTextures(LKengine::GLTexture texture)
{
	// Player texture
	texture = LKengine::ResourceManager::getTexture("Textures/Player/ct1.png");
	m_texturePlayer.init(texture, glm::ivec2(3, 2));

	// Players legs texture
	texture = LKengine::ResourceManager::getTexture("Textures/Player/legs.png");
	m_textureLegs.init(texture, glm::ivec2(4, 4));

	// Gun Textures
	// AK texture
	texture = LKengine::ResourceManager::getTexture("Textures/Weapons/ak47.png");
	m_textureAK47.init(texture, glm::ivec2(1, 1));
	// M3 shotgun texture
	texture = LKengine::ResourceManager::getTexture("Textures/Weapons/m3.png");
	m_textureM3.init(texture, glm::ivec2(1, 1));
	// Deagle pistol texture
	texture = LKengine::ResourceManager::getTexture("Textures/Weapons/deagle.png");
	m_textureDeagle.init(texture, glm::ivec2(1, 1));
	// Smoke grenade texture
	texture = LKengine::ResourceManager::getTexture("Textures/Weapons/smokegrenade2.png");
	m_textureSmoke.init(texture, glm::ivec2(1, 1));

	// Magazine texture
	texture = LKengine::ResourceManager::getTexture("Textures/Weapons/mag.png");
	m_magazineTexture.init(texture, glm::ivec2(1, 1));

}


void Player::initParticleSystems(LKengine::GLTexture texture)
{
	// Initialise particles
	// Load blood sprite sheet
	texture = LKengine::ResourceManager::getTexture("Textures/Misc/smoke_particle.png");
	// Set the size of decal sprite sheet
	m_smokeTexture.init(texture, glm::ivec2(1, 1));

	m_grenadeParticleBatch = new LKengine::ParticleBatch2D();
	m_grenadeParticleBatch->init(500, 0.005f,
		m_smokeTexture,
		0,
		[](LKengine::Particle2D& particle, float deltaTime)
	{
		particle.position += particle.velocity * deltaTime;
		particle.color.a = (GLubyte)(particle.life * 255.0f);
	});

	m_particleEngine.addParticleBatch(m_grenadeParticleBatch);
}


void Player::addGun(Gun* gun)
{
	//add gun to player inventory
	m_guns.push_back(gun);


	//if no gun equiped, equip gun
	if (m_currentGunIndex == -1)
	{
		m_currentGunIndex = 0;
	}

}


void Player::update(const std::vector< std::vector<std::string> >& levelData, float deltaTime)
{
	// Variables needed for updating the player and the players items
	glm::vec2 mouseCoords = m_inputManager->getMouseCoords();
	mouseCoords = m_camera->convertScreenToWorld(mouseCoords);

	glm::vec2 centrePosition = m_position + glm::vec2(PLAYER_RADIUS);

	glm::vec2 gunOffset;
	gunOffset.x = (GUN_OFFSET + GUN_LENGTH * m_direction.x);
	gunOffset.y = (GUN_OFFSET + GUN_LENGTH * m_direction.y);
	glm::vec2 gunEndPosition;
	gunEndPosition.x = centrePosition.x + ((GUN_OFFSET + GUN_LENGTH) * m_direction.x);
	gunEndPosition.y = centrePosition.y + ((GUN_OFFSET + GUN_LENGTH) * m_direction.y);

	m_direction = glm::normalize(mouseCoords - centrePosition);

	//std::cout << "m_direction.x = " << m_direction.x << " m_direction.y = " << m_direction.y << std::endl;
	//std::cout << "m_legDirection.x = " << m_legDirection.x << " m_legDirection.y = " << m_legDirection.y << std::endl;

	//std::cout << "dot(m_direction, m_legDirection)  = " << glm::dot(m_direction, m_legDirection) << std::endl;

	std::cout << "m_speed  = " << m_speed << std::endl;

	// Update the players movement
	updatePlayerMovement(deltaTime);

	// Update everything to do with reloading the guns
	updateReloading();

	// Update weapon swapping
	updateWeaponSwap();

	// Update shooting
	updateShooting(mouseCoords, centrePosition, gunEndPosition, deltaTime);
	
	// Update grenades
	// Check if a grenade is thrown
	updateThrowingGrenades(mouseCoords, centrePosition, deltaTime);
	// Update grenades that have been thrown
	updateGrenades(levelData, deltaTime);


	// Check if footstep sound should be played
	playFootsteps();

	// Update particle engine of smoke
	m_particleEngine.update(deltaTime);

	// Update the magazines
	updateMagazines(deltaTime);

	// Player collision detection with world
	collideWithLevel(levelData);

}


bool Player::collideWithLevel(const std::vector< std::vector<std::string> >& levelData)
{
	std::vector<glm::vec2> collideTilePosition;

	//check the 4 corners
	//first corner
	checkTilePosition(levelData, collideTilePosition,
		m_position.x, m_position.y);
	//second corner
	checkTilePosition(levelData, collideTilePosition,
		m_position.x + PLAYER_WIDTH, m_position.y);
	//third corner
	checkTilePosition(levelData, collideTilePosition,
		m_position.x, m_position.y + PLAYER_WIDTH);
	//fourth corner
	checkTilePosition(levelData, collideTilePosition,
		m_position.x + PLAYER_WIDTH, m_position.y + PLAYER_WIDTH);

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


void Player::draw(LKengine::SpriteBatch& spriteBatch)
{
	glm::vec4 destRectPlayer;
	destRectPlayer.x = m_position.x;
	destRectPlayer.y = m_position.y;
	destRectPlayer.z = PLAYER_WIDTH;
	destRectPlayer.w = PLAYER_WIDTH;

	glm::vec4 destRectLegs;
	destRectLegs.x = destRectPlayer.x -(((PLAYER_WIDTH / 10) *1.5) * m_direction.x);
	destRectLegs.y = destRectPlayer.y -(((PLAYER_WIDTH / 10) *1.4) * m_direction.y);
	destRectLegs.z = PLAYER_WIDTH;
	destRectLegs.w = PLAYER_WIDTH;

	glm::vec4 destRectGun;
	destRectGun.x = m_position.x + (GUN_OFFSET * m_direction.x);
	destRectGun.y = m_position.y + (GUN_OFFSET * m_direction.y);
	destRectGun.z = PLAYER_WIDTH;
	destRectGun.w = PLAYER_WIDTH;




	// Player RELOADING
	if (m_playerReloading)
	{
		m_tileIndexPlayer = 5;
		m_numTilesPlayer = 1;
		m_animationSpeed *= 0.25f;
		if (m_moveState != PlayerMoveState::RELOADING)
		{
			m_moveState = PlayerMoveState::RELOADING;
			m_animationTime = 0.0f;
		}
	}
	// Player holding a 2 handed weapon (eg rifle)
	else if (m_playerReloading == false && m_currentGunIndex != 2)
	{
		m_tileIndexPlayer = 3;
		m_numTilesPlayer = 1;
		if (m_moveState != PlayerMoveState::RUNNING)
		{
			m_moveState = PlayerMoveState::RUNNING;
			m_animationTime = 0.0f;
		}
	}
	// Player holding a single handed weapon (eg single pistol)
	else if (m_playerReloading == false && m_currentGunIndex == 2)
	{
		m_tileIndexPlayer = 1;
		m_numTilesPlayer = 1;
		if (m_moveState != PlayerMoveState::RUNNING)
		{
			m_moveState = PlayerMoveState::RUNNING;
			m_animationTime = 0.0f;
		}
	}


	if (m_playerSprinting == true) {
		m_animationSpeedLegs = 0.25f;
	} else {
		m_animationSpeedLegs = 0.15f;
	}


	// Player legs animation
	if (m_playerMoving == true)
	{
		m_tileIndexLegs = 0;
		m_numTilesLegs = 10;
		if (m_moveState != PlayerMoveState::RUNNING)
		{
			m_moveState = PlayerMoveState::RUNNING;
			m_animationTimeLegs = 0.0f;
		}
	}
	else{
		m_tileIndexLegs = 0;
		m_numTilesLegs = 1;
		if (m_moveState != PlayerMoveState::STANDING)
		{
			m_moveState = PlayerMoveState::STANDING;
			m_animationTimeLegs = 0.0f;
			footstepAudioPlay = true;
		}
	}




	// Increment animation time
	m_animationTime += m_animationSpeed;
	m_animationTimeLegs += m_animationSpeedLegs;

	// Check for legs animation end
	if (m_animationTimeLegs > m_numTilesLegs)
	{
		m_animationTimeLegs = 0.0f;
	}
	// Check for animation end
	if (m_animationTime > m_numTilesPlayer)
	{
		m_animationTime = 0.0f;
	}

	if (m_playerReloading)
	{
		if (m_reloadCounter == 1)
		{
			m_clipOutSound.play();
		}
		if (m_reloadCounter == 100)
		{
			m_clipInSound.play();
			m_reloadCounter = 0;
			m_playerReloading = false;
		}
		m_reloadCounter++;
	}


	// Apply animation
	m_tileIndexPlayer = m_tileIndexPlayer + (int)m_animationTime % m_numTilesPlayer;
	m_tileIndexLegs = m_tileIndexLegs + (int)m_animationTimeLegs % m_numTilesLegs;
	m_tileIndexGun = m_tileIndexGun + (int)m_animationTime % m_numTilesGun;


	// Get the uv coordinates from the tile index
	glm::vec4 uvRectPlayer = m_texturePlayer.getUVs(m_tileIndexPlayer);
	glm::vec4 uvRectLegs = m_textureLegs.getUVs(m_tileIndexLegs);
	glm::vec4 uvRectAK47 = m_textureAK47.getUVs(m_tileIndexGun);
	glm::vec4 uvRectM3 = m_textureM3.getUVs(m_tileIndexGun); 
	glm::vec4 uvRectDeagle = m_textureDeagle.getUVs(m_tileIndexGun); 


	// Draw the player sprite
	spriteBatch.draw(destRectPlayer, uvRectPlayer, m_texturePlayer.texture.id, DEPTH_PLAYER, m_color, m_direction);
	// Draw the legs sprite
	spriteBatch.draw(destRectLegs, uvRectLegs, m_textureLegs.texture.id, DEPTH_LEGS, m_color, m_direction);

	// Draw primary weapon
	if(m_currentGunIndex == 0)
		spriteBatch.draw(destRectGun, uvRectAK47, m_textureAK47.texture.id, DEPTH_GUN, m_color, m_direction);
	// Draw secondary weapon
	if(m_currentGunIndex == 1)
		spriteBatch.draw(destRectGun, uvRectM3, m_textureM3.texture.id, DEPTH_GUN, m_color, m_direction);
	// Draw third weapon
	if (m_currentGunIndex == 2)
		spriteBatch.draw(destRectGun, uvRectDeagle, m_textureDeagle.texture.id, DEPTH_GUN, m_color, m_direction);


	// Draw dropped magazines
	drawMagazine(spriteBatch);

	// Draw grenades
	for (int i = 0; i < m_grenades.size(); i++)
	{
		m_grenades[i].draw(spriteBatch);
	}
	
	m_grenadeParticleBatch->draw(&spriteBatch);

}


void Player::drawDebug(LKengine::DebugRenderer& debugRenderer)
{
	LKengine::ColorRGBA8 color(255, 255, 255, 255);

	glm::vec4 destRect;
	destRect.x = m_position.x;
	destRect.y = m_position.y;
	destRect.z = PLAYER_WIDTH;
	destRect.w = PLAYER_WIDTH;

	// Draw circle
	debugRenderer.drawCircle(glm::vec2(destRect.x + PLAYER_RADIUS, destRect.y + PLAYER_RADIUS), color, PLAYER_RADIUS);

	for (int i = 0; i < m_grenades.size(); i++)
	{
		m_grenades[i].drawDebug(debugRenderer);
	}

}


bool Player::applyDamage(int damage)
{
	// Play sound effect when player is hit
	static std::mt19937 randomEngine(time(nullptr));
	//get random sound effect number between 1 and 3
	std::uniform_int_distribution<int> soundEffectNumber(1, 2);

	if (soundEffectNumber(randomEngine) == 1) {
		m_playerHurt1.play();
	}
	if (soundEffectNumber(randomEngine) == 2) {
		m_playerHurt2.play();
	}

	// Check if we have any armour to apply damage to
	if (m_armour > 0)
	{
		// If we have enough armour to take all the damage, remove the damage value from armour
		if (m_armour >= damage)
		{
			m_armour -= damage;
			return false;
		}
		// If have some armour but not enough to take all the damage, reduce armour and keep track of damage to apply to healt
		else
		{
			damage -= m_armour;
			m_armour = 0;
		}
	}

	// If there is still damage to be applied to player health after armour removal
	if (damage > 0) {
		m_health -= damage;
	}

	if (m_health < 0) {
		m_health = 0;
	}


//	m_armour -= damage;
//	m_health -= damage;

	if (m_health <= 0)
	{
		return true;
	}
	return false;
}


void Player::addPlayerMoney(int money)
{
	m_money += money;
}


void Player::drawPlayer()
{

}


void Player::drawGun()
{

}

void Player::drawGrenade()
{

}


void Player::drawMagazine(LKengine::SpriteBatch& spriteBatch)
{
	glm::vec4 destRect;
	glm::vec4 uvRect;

	// Cycle through the magazines dropped on floor and draw them
	for (int i = 0; i < m_decalsMags.size(); i++)
	{
		destRect = glm::vec4(m_decalsMags[i]->getPosition().x, m_decalsMags[i]->getPosition().y, m_decalsMags[i]->getWidth(), m_decalsMags[i]->getHeight());

		uvRect = m_magazineTexture.getUVsTopLeft(m_decalsMags[i]->getTileID());

		// Draw the magazines
		spriteBatch.draw(destRect, uvRect, m_magazineTexture.texture.id, DEPTH_MAGAZINES, m_decalsMags[i]->getColor());
	}
}


void Player::updatePlayerMovement(float deltaTime)
{
	glm::fvec2 velocity(0, 0);
	m_speed = PLAYER_FORWARD_SPEED;

	// Player movement
	// Up
	if (m_inputManager->isKeyDown(SDLK_w))
	{
		velocity.y += 1;
		m_playerMoving = true;
		m_legDirection = glm::vec2(0.0f,1.0f);
	}
	// Down
	else if (m_inputManager->isKeyDown(SDLK_s))
	{
		velocity.y -= 1;
		m_playerMoving = true;
		m_legDirection = glm::vec2(0.0f, -1.0f);
	}
	// Left
	if (m_inputManager->isKeyDown(SDLK_a))
	{
		velocity.x -= 1;
		m_playerMoving = true;
		m_legDirection = glm::vec2(-1.0f, 0.0f);
	}
	// Right
	else if (m_inputManager->isKeyDown(SDLK_d))
	{
		velocity.x += 1;
		m_playerMoving = true;
		m_legDirection = glm::vec2(1.0f, 0.0f);

	}
	// DIAGONALS
	// Up Left
	if (m_inputManager->isKeyDown(SDLK_w) && m_inputManager->isKeyDown(SDLK_a))
	{
		m_legDirection = glm::vec2(-0.7f, 0.7f);
	}
	// Up Right
	if (m_inputManager->isKeyDown(SDLK_w) && m_inputManager->isKeyDown(SDLK_d))
	{
		m_legDirection = glm::vec2(0.7f, 0.7f);
	}
	// Down Left
	if (m_inputManager->isKeyDown(SDLK_s) && m_inputManager->isKeyDown(SDLK_a))
	{
		m_legDirection = glm::vec2(-0.7f, -0.7f);
	}
	// Down Right
	if (m_inputManager->isKeyDown(SDLK_s) && m_inputManager->isKeyDown(SDLK_d))
	{
		m_legDirection = glm::vec2(0.7f, -0.7f);
	}


	if ((m_inputManager->isKeyDown(SDLK_w)) == false && (m_inputManager->isKeyDown(SDLK_s)) == false && (m_inputManager->isKeyDown(SDLK_a)) == false && (m_inputManager->isKeyDown(SDLK_d)) == false)
	{
		m_playerMoving = false;
	}

	// Check dot product of direction player is facing and direction legs are moving to get a speed player should run
	// Looking the way the player is walking (walking forwards)
	if (glm::dot(m_direction, m_legDirection) >= 0.8)
	{
		m_speed = PLAYER_FORWARD_SPEED;
	}
	// Walking forwards diagonal
	else if (glm::dot(m_direction, m_legDirection) < 0.8 && glm::dot(m_direction, m_legDirection) >= 0.4)
	{
		m_speed = PLAYER_FORWARD_DIAGONAL_SPEED;
	}
	// Sideways strafing
	else if(glm::dot(m_direction, m_legDirection) < 0.4 && glm::dot(m_direction, m_legDirection) > -0.4)
	{
		m_speed = PLAYER_STRAFE_SPEED;
	}
	// walking forwards diagonal
	else if (glm::dot(m_direction, m_legDirection) > -0.8 && glm::dot(m_direction, m_legDirection) <= -0.4)
	{
		m_speed = PLAYER_BACKWARDS_DIAGONAL_SPEED;
	}
	// Looking opposite way the player is walking (walking backwards)
	else if (glm::dot(m_direction, m_legDirection) <= -0.8)
	{
		m_speed = PLAYER_BACKWARDS_SPEED;
	}


	// Sprinting
	if (m_inputManager->isKeyDown(SDLK_LSHIFT))
	{
		// Mulitply the speed by the player sprint speed value
		m_speed = m_speed * PLAYER_SPRINT_SPEED;
		m_playerSprinting = true;
	}
	else
	{
		m_playerSprinting = false;
	}


	// Normalise the velocity to cap it to 1, even if going diagonally
	double length = sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));
	// Will != 0 when movement is input so updates the players position
	if (length != 0)
	{
		velocity.x /= length;
		velocity.y /= length;

		velocity.x *= m_speed * deltaTime;
		velocity.y *= m_speed * deltaTime;

		m_position.x += velocity.x;
		m_position.y += velocity.y;
	}

}


void Player::updateReloading()
{
	// Reloading
	// To reload there needs to be: "R" pressed, Player not already reloading, Ammo missing from magazine, ammo left to reload into mag.
	if (m_inputManager->isKeyDown(SDLK_r) && m_playerReloading == false && m_guns[m_currentGunIndex]->reloadNeeded == true && m_guns[m_currentGunIndex]->reloadAllowed() == true)
	{
		// Calls reloadMag which takes ammoLeft and puts into magLeft
		m_guns[m_currentGunIndex]->reloadMag();
		// Sets the player to reloading
		m_playerReloading = true;
		// Sets reloadNeeded to false so can't reload again when magazine full
		m_guns[m_currentGunIndex]->reloadNeeded = false;

		static std::mt19937 randEngine(time(nullptr));
		static std::uniform_real_distribution<float> randVel(0.2f, 0.4);
		float angle = 0;
		float lifeTotal = 10;
		float decayRate = 0.01f;
		float widthMag = 15.0f;
		float heightMag = 15.0f;
		// Add magazine on floor as reloading

		m_decalsMags.push_back(new Decal(m_position + PLAYER_RADIUS, glm::vec2(randVel(randEngine) * m_direction.x, randVel(randEngine) * m_direction.y), 0, lifeTotal, decayRate, LKengine::ColorRGBA8(255, 255, 255, 255), widthMag, heightMag, m_magazineTexture, 0));

		// Debug messages
		std::cout << "*****************************R PRESSED**********************************" << std::endl;
		std::cout << "MagLeft: " << m_guns[m_currentGunIndex]->getMagLeft() << std::endl;
		std::cout << "AmmoTotalLeft: " << m_guns[m_currentGunIndex]->getAmmoTotalLeft() << std::endl;
	}
}


void Player::updateWeaponSwap()
{
	// Primary weapon
	if (m_inputManager->isKeyDown(SDLK_1) && m_guns.size() >= 0)
	{
		m_currentGunIndex = 0;
		GUN_OFFSET = 20;
		GUN_LENGTH = 5;
	}
	// Secondary weapon
	else if (m_inputManager->isKeyDown(SDLK_2) && m_guns.size() >= 1)
	{
		m_currentGunIndex = 1;
		GUN_OFFSET = 20;
		GUN_LENGTH = 5;
	}
	// Third weapon
	else if (m_inputManager->isKeyDown(SDLK_3) && m_guns.size() >= 2)
	{
		m_currentGunIndex = 2;
		GUN_OFFSET = 27.5;
		GUN_LENGTH = 5;
	}
}


void Player::updateShooting(glm::vec2 mouseCoords, glm::vec2 centrePosition, glm::vec2 gunEndPosition, float deltaTime)
{
	m_frameCounterShooting += 1.0f * deltaTime;

	// To Shoot: Left mouse btn pressed, player isn't reloading, ammo in magazine
	if (m_inputManager->isKeyDown(SDL_BUTTON_LEFT) && m_playerReloading == false && m_guns[m_currentGunIndex]->getMagLeft() > 0)
	{
		m_playerShooting = true;
		m_guns[m_currentGunIndex]->reloadNeeded = true;
	}

	// To Shoot: Gun selected, player isn't reloading and ammo left in magazine
	if (m_currentGunIndex != -1 && m_playerReloading == false && m_guns[m_currentGunIndex]->getMagLeft() > 0)
	{
		m_guns[m_currentGunIndex]->update(m_inputManager->isKeyDown(SDL_BUTTON_LEFT),
			gunEndPosition,
			m_direction,
			*m_bullets,
			m_frameCounterShooting,
			deltaTime);
	}

	// Player empty gun sound if no ammo left to shoot
	if (m_currentGunIndex != -1 && m_playerReloading == false && m_guns[m_currentGunIndex]->getMagLeft() == 0)
	{
		m_guns[m_currentGunIndex]->playEmptyMagSound(m_inputManager->isKeyDown(SDL_BUTTON_LEFT), m_emptyMagazine, m_frameCounterShooting, deltaTime);
	}




}


void Player::updateMagazines(float deltaTime)
{
	for (int i = 0; i < m_decalsMags.size();)
	{
		// Update position
		glm::vec2 position = m_decalsMags[i]->getPosition() + m_decalsMags[i]->getVelocity() * deltaTime;
		m_decalsMags[i]->setPosition(position);

		// Update velocity
		glm::vec2 velocity = m_decalsMags[i]->getVelocity();
		velocity *= std::min((m_decalsMags[i]->getLifeLeft() / 10), 1.0f);
		m_decalsMags[i]->setVelocity(velocity);

		// Update lifeleft
		float lifeLeft = m_decalsMags[i]->getLifeLeft() - m_decalsMags[i]->getDecayRate() * deltaTime;
		m_decalsMags[i]->setLifeLeft(lifeLeft);

		// Update color
		LKengine::ColorRGBA8 color(255, 255, 255, 255);
		// Stop flickering by multiplying Alpha with 1 or when lifeLeft is less than 1
		color.a = std::min(m_decalsMags[i]->getLifeLeft(), 1.0f) * 255.0f;
		m_decalsMags[i]->setColor(color);

		// Remove decal if no longer "alive"
		if (m_decalsMags[i]->getLifeLeft() <= 0)
		{
			m_decalsMags[i] = m_decalsMags.back();
			m_decalsMags.pop_back();
		}
		else
		{
			i++;
		}

	}
}


void Player::updateThrowingGrenades(glm::vec2 mouseCoords, glm::vec2 centrePosition, float deltaTime)
{
	// Throwing Grenade
	// To throw a grenade: Left mouse btn pressed, grenade in player inventory, ammo grenade is equipped
	if (m_inputManager->isKeyDown(SDLK_g) && m_numGrenades > 0 && m_grenadeThrown == false)
	{
		glm::vec2 grenadePosition;
		float GRENADE_OFFSET = 25.0f;
		grenadePosition.x = centrePosition.x - GRENADE_WIDTH/2 + GRENADE_OFFSET * m_direction.x;
		grenadePosition.y = centrePosition.y - GRENADE_WIDTH/2 + GRENADE_OFFSET * m_direction.y;

		m_grenades.emplace_back("Smoke Grenade", grenadePosition, m_direction, mouseCoords, m_textureSmoke, 0);



		m_numGrenades--;

		m_grenadeThrown = true;
		m_grenadeCooldown = 0;

		std::cout << "G Pressed \n";
	}

	if (m_grenadeCooldown < 5.0f && m_grenadeThrown == true) {
		m_grenadeCooldown += 0.08f * deltaTime;

	}
	else {
		m_grenadeThrown = false;
	}

}


void Player::updateGrenades(const std::vector< std::vector<std::string> >& levelData, float deltaTime)
{
	// Update grenades
	for (int i = 0; i < m_grenades.size(); )
	{

		if (m_grenades[i].getTimePassed() < m_grenades[i].getTimer())
		{
			std::cout << m_grenades[i].getTimePassed() << std::endl;
			m_grenades[i].update(levelData, deltaTime);
			i++;
		}
		else
		{
			addSmokeGrenadeSmoke(m_grenades[i].getPosition(), 80);
			m_grenades[i] = m_grenades.back();
			m_grenades.pop_back();
		}
	}

}


void Player::addSmokeGrenadeSmoke(const glm::vec2& position, int numParticles)
{
	static std::mt19937 randEngine(time(nullptr));
	static std::uniform_real_distribution<float> randAngle(0.0f, 2.0f * M_PI);

	glm::vec2 vel(0.1f, 0.0f);
	LKengine::ColorRGBA8 col(50, 50, 50, 255);

	for (int i = 0; i < numParticles; i++) {
		m_grenadeParticleBatch->addParticle(
			position + randAngle(randEngine),
			glm::rotate(vel, randAngle(randEngine)),
			0.0f,
			col,
			50.0f);
	}
}


void Player::checkTilePosition(
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
void Player::collideWithTile(glm::vec2 tilePos)
{

	const float TILE_RADIUS = (float)TILE_WIDTH / 2.0f;
	// The minimum distance before a collision occurs
	const float MIN_DISTANCE = PLAYER_RADIUS + TILE_RADIUS;

	// Centre position of the agent
	glm::vec2 centrePlayerPos = m_position + glm::vec2(PLAYER_RADIUS);
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


void Player::playFootsteps()
{
	static std::mt19937 randomEngine(time(nullptr));
	//get random sound effect number between 1 and 3
	std::uniform_int_distribution<int> soundEffectNumber(1, 3);

	if (soundEffectNumber(randomEngine) == 1) {
		m_footstep = m_footstepGrass1;
	}
	if (soundEffectNumber(randomEngine) == 2) {
		m_footstep = m_footstepGrass2;
	}
	if (soundEffectNumber(randomEngine) == 3) {
		m_footstep = m_footstepGrass3;
	}

	// Footstep Audio
	// Divide animation frame (m_numTilesLegs) by 10 to find a range to determin if footstep has been made
	float number = m_numTilesLegs / 10;
	if (m_animationTimeLegs > number * 2 && m_animationTimeLegs < number * 3 && footstepAudioPlay == true)
	{
		m_footstep.play();
		footstepAudioPlay = false;
	}
	if (m_animationTimeLegs > number * 7 && m_animationTimeLegs < number * 8 && footstepAudioPlay == false)
	{

		m_footstep.play();
		footstepAudioPlay = true;
	}

}