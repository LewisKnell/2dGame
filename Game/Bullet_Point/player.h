#pragma once

#include "bullet.h"
#include "decal.h"
#include "gun.h"
#include "grenade.h"
#include "level.h"

#include <LKengine\audioEngine.h>
#include <LKengine\inputManager.h>
#include <LKengine\camera2D.h>
#include <LKengine\particleEngine2D.h>
#include <LKengine\particleBatch2D.h>

#include <LKengine\debugRenderer.h>

const float PLAYER_WIDTH = 45.0f; // 60 original
const float PLAYER_RADIUS = PLAYER_WIDTH / 2.0f;

const float DEPTH_MAGAZINES = -0.08f;
const float DEPTH_LEGS = -0.09f;
const float DEPTH_PLAYER = -1.0f;
const float DEPTH_GUN = -1.1f;

//const float PLAYER_SPEED = 4.0f;
const float PLAYER_FORWARD_SPEED = 4.0f;
const float PLAYER_FORWARD_DIAGONAL_SPEED = 3.5f;
const float PLAYER_STRAFE_SPEED = 3.0f;
const float PLAYER_BACKWARDS_DIAGONAL_SPEED = 2.5f;
const float PLAYER_BACKWARDS_SPEED = 2.0f;
const float PLAYER_SPRINT_SPEED = 1.5f; // Multiply speed by this
const int MAX_CAPACITY = 100;

//class Gun; ///< forward declare

enum class PlayerMoveState { STANDING, WALKING, RUNNING, SHOOTING, RELOADING, MELEE };

class Player
{
public:
	Player();
	~Player();

	// Initialise the player and their variables
	void init(glm::vec2 pos, LKengine::InputManager* inputManager, LKengine::Camera2D* camera,	std::vector<Bullet>* bullets);
	// Initialise the players guns
	void initGuns();
	// Initialise the sounds
	void initSounds();
	// Initialise the textures
	void initTextures(LKengine::GLTexture texture);
	// Initialise the particle systems
	void initParticleSystems(LKengine::GLTexture texture);

	void addGun(Gun* gun);

	void update(const std::vector< std::vector<std::string> >& levelData, float deltaTime);

	bool collideWithLevel(const std::vector< std::vector<std::string> >& levelData);

	// Main draw fuction in player class
	void draw(LKengine::SpriteBatch& spriteBatch);
	// Draws a debug circle around the Player the size of their collision detection
	void drawDebug(LKengine::DebugRenderer& debugRenderer);

	// Return true if we died
	bool applyDamage(int damage);

	// Getters
	glm::vec2 getPlayerPosition() { return m_position; }
	int getAmmoTotalLeft() { return m_guns[m_currentGunIndex]->getAmmoTotalLeft();	}
	int getMagLeft() { return m_guns[m_currentGunIndex]->getMagLeft(); }
	int getPlayerHealth() const { return m_health; }
	int getPlayerArmour() const { return m_armour; }
	int getPlayerMoney() const { return m_money; }
	float getPlayerSpeed() const { return m_speed; }

	// Setters
	void setPlayerPosition(glm::vec2 position) { m_position += position; }

	// Add money to player. Input money added to m_money
	void addPlayerMoney(int money);


protected:

private:
	// Collision detection methods
	void checkTilePosition(const std::vector< std::vector<std::string> >& levelData, std::vector<glm::vec2>& collideTilePosition, float x, float y);
	void collideWithTile(glm::vec2 tilePos);

	// Draw the player sprite and their animations
	void drawPlayer();
	// Draw the players gun
	void drawGun();
	// Draw the grenades
	void drawGrenade();
	// Draw the magazines droped when reloading
	void drawMagazine(LKengine::SpriteBatch& spriteBatch);

	// Checks if player is moving and update the players movement
	void updatePlayerMovement(float deltaTime);
	// Checks if reloading and update everything to do with reloading the guns
	void updateReloading();
	// Checks for weapon swapping
	void updateWeaponSwap();
	// Checks if shooting and updates
	void updateShooting(glm::vec2 mouseCoords, glm::vec2 centrePosition, glm::vec2 gunEndPosition, float deltaTime);
	// Update the magazines dropped on the floor
	void updateMagazines(float deltaTime);
	// Check if a grenade is thrown
	void updateThrowingGrenades(glm::vec2 mouseCoords, glm::vec2 centrePosition, float deltaTime);
	// Update grenades that have been thrown
	void updateGrenades(const std::vector< std::vector<std::string> >& levelData, float deltaTime);

	// Add smoke grenade smoke when timer is 0
	void addSmokeGrenadeSmoke(const glm::vec2& position, int numParticles);

	// Play footstep audio
	void playFootsteps();


	LKengine::InputManager* m_inputManager;
	LKengine::Camera2D* m_camera;

	LKengine::AudioEngine m_audioEngine;

	// Sound Effects
	// Gun sound effects
	// Reload sound effects
	LKengine::SoundEffect m_clipOutSound;
	LKengine::SoundEffect m_clipInSound;
	// Empty magazine sound effect
	LKengine::SoundEffect m_emptyMagazine;
	// Footstep sound effects
	LKengine::SoundEffect m_footstep;
	LKengine::SoundEffect m_footstepGrass1;
	LKengine::SoundEffect m_footstepGrass2;
	LKengine::SoundEffect m_footstepGrass3;
	//Misc sound effects
	LKengine::SoundEffect m_playerHurt1;
	LKengine::SoundEffect m_playerHurt2;

	bool footstepAudioPlay = true;


	LKengine::ParticleEngine2D m_particleEngine;
	LKengine::ParticleBatch2D* m_grenadeParticleBatch;
	
	// Textures
	LKengine::TileSheet m_smokeTexture;
	LKengine::TileSheet m_magazineTexture;
	LKengine::TileSheet m_texturePlayer;
	LKengine::TileSheet m_textureLegs;
	LKengine::TileSheet m_textureAK47;
	LKengine::TileSheet m_textureM3;
	LKengine::TileSheet m_textureDeagle;
	LKengine::TileSheet m_textureSmoke;


	std::vector<Gun*> m_guns;
	std::vector<Bullet>* m_bullets;
	std::vector<Grenade> m_grenades;
	std::vector<Decal*> m_decalsMags;


	// Player variables
	glm::vec2 m_position;
	glm::vec2 m_direction = glm::vec2(1.0f, 0.0f);
	int m_health = 0;
	int m_armour = 0;
	float m_speed = 0.0f;
	int m_money = 0;
	int m_capacity = 0;
	float m_stamina = 0.0f;
	glm::vec2 m_legDirection = glm::vec2(1.0f, 1.0f);

	LKengine::ColorRGBA8 m_color;
	PlayerMoveState m_moveState = PlayerMoveState::STANDING;

	int m_currentGunIndex = 0;
	int m_numGrenades = 10;
	float m_grenadeCooldown = 0;
	bool m_grenadeThrown = false;

	// Animation variables
	float m_animationSpeed = 0.001f;
	float m_animationSpeedLegs = 0.15f;

	float m_animationTime = 0.0f;
	float m_animationTimeLegs = 0.0f;

	int m_tileIndexPlayer = 0;
	int m_tileIndexLegs = 0;
	int m_tileIndexGun = 0;

	int m_numTilesPlayer = 0;
	int m_numTilesLegs = 6;
	int m_numTilesGun = 1;

	int m_reloadCounter = 0;

	bool m_playerMoving = false;
	bool m_playerReloading = false;
	bool m_playerShooting = false;
	bool m_playerMelee = false;
	bool m_playerSprinting = false;

	float m_frameCounterShooting = 0;

	float GUN_OFFSET = 0; ///< Offset for gun texture to be in players hands
	float GUN_LENGTH = 0; ///< Offset for length of the gun

};

