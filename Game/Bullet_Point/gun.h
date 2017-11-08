#pragma once

#include <glm\glm.hpp>

#include <string>
#include <vector>

#include <LKengine\audioEngine.h>

#include "bullet.h"

const int RIFLE_WEIGHT = 30;
const int SHOTGUN_WEIGHT = 20;
const int PISTOL_WEIGHT = 10;

class Gun
{
public:
	Gun(std::string name, int fireRate, int bulletsPerShot, 
		float spread, float bulletDamage, float bulletSpeed, 
		int ammoTotal, int magSize, int weight, LKengine::SoundEffect fireEffect);

	~Gun();

	void update(bool isMouseDown, const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets, float& frameCounter, float deltaTime);

	// Getters
	int getAmmoTotalLeft() { return m_ammoTotal;  }
	int getMagLeft() { return m_magLeft; }
	int getWeight() { return m_weight; }

	bool reloadAllowed();

	void updateAmmo();
	void reloadMag();

	void playEmptyMagSound(bool isMouseDown, LKengine::SoundEffect& emptyMagazine, float& frameCounter, float deltaTime);

	bool reloadNeeded = false;
	

protected:
	int m_ammoTotal; ///< Total number of rounds player can hold of the gun
	int m_magSize; ///< Max ammo a magazine can hold
	int m_magLeft; ///< Ammo left in the current magazine (reduces with each shot)
	int m_weight; ///< How much inventory space the gun takes up


private:

	void shoot(const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets);


	LKengine::SoundEffect m_fireEffect;


	std::string m_name;

	int m_fireRate; ///< Fire rate in terms of frames
	int m_bulletsPerShot; ///< How many bullets are fired at a time
	float m_spread; ///< Accuracy
	float m_bulletDamage;
	float m_bulletSpeed; 

	//float m_frameCounter; ///< Count frames so we know when to shoot

	bool m_reloadNeeded = false;
};

