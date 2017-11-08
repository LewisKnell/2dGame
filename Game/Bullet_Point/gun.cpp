#include "gun.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\rotate_vector.hpp>
#include <random>
#include <ctime>

#include <LKengine\resourceManager.h>

#include <iostream> //< Delete when no longer debugging / refactoring

Gun::Gun(std::string name, int fireRate, int bulletsPerShot, 
	float spread, float bulletDamage, float bulletSpeed, 
	int ammoTotal, int magSize, int weight, LKengine::SoundEffect fireEffect) :
	m_name(name),
	m_fireRate(fireRate),
	m_bulletsPerShot(bulletsPerShot),
	m_spread(spread),	
	m_bulletSpeed(bulletSpeed),
	m_bulletDamage(bulletDamage),
	//m_frameCounter(0),
	m_fireEffect(fireEffect),
	m_ammoTotal(ammoTotal),
	m_magSize(magSize),
	m_weight(weight)

{
	m_magLeft = magSize;

}


Gun::~Gun() {
	//Empty
}


void Gun::update(bool isMouseDown, const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets, float& frameCounter, float deltaTime)
{
	if (frameCounter >= m_fireRate && isMouseDown)
	{
		shoot(position, direction, bullets);
		frameCounter = 0;
	}
}




bool Gun::reloadAllowed() 
{
	// check if we have ammo left to reload magazine
	if (m_ammoTotal >= m_magSize - m_magLeft)
	{
		return true;
	}
	// If we don't have ammo to reload, return the current values.
	else 
	{
		return false;
	}
}



void Gun::updateAmmo()
{
	m_magLeft--;
}


void Gun::reloadMag()
{
	// Subtract the ammo that is needed to fill the mag back to full mag
	m_ammoTotal -= m_magSize - m_magLeft;

	// MagLeft goes back to full mag value
	m_magLeft = m_magSize;

	// Ammo Debug
	std::cout << "MagLeft: " << getMagLeft() << std::endl;
	std::cout << "AmmoTotalLeft: " << getAmmoTotalLeft() << std::endl;
}


void Gun::playEmptyMagSound(bool isMouseDown, LKengine::SoundEffect& emptyMagazine, float& frameCounter, float deltaTime)
{
	if (frameCounter >= m_fireRate && isMouseDown)
	{
		emptyMagazine.play();
		frameCounter = 0;
	}
}


void Gun::shoot(const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets)
{
	static std::mt19937 randomEngine(time(nullptr));
	//get random direction using the _spread (accuracy) variable
	std::uniform_real_distribution<float> randRotate(-m_spread * 3.14159265359f / 180, m_spread * 3.14159265359f / 180);

	m_fireEffect.play();

	for (int i = 0; i < m_bulletsPerShot; i++)
	{
		bullets.emplace_back(position, glm::rotate(direction, randRotate(randomEngine)), m_bulletDamage, m_bulletSpeed);
	}

	// Update ammo to remove bullet fired from magazine
	updateAmmo();

}

