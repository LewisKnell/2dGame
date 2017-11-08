#pragma once

#include <string>
#include <vector>

#include "level.h"

class Zombies;

class Waves
{
public:
	Waves();
	~Waves();

	void init(const std::string& fileName);

	void generateWave(const std::vector<glm::vec2>& zombieStartPosition, std::vector<Zombie*>& m_zombies);
	

	// Getters
	int getNumWaves() { return m_numWaves; }
	int getCurrentWaveNumber() { return m_currentWaveNumber; }
	int getWaveDataZombies(int waveNumber) { return m_waveData[waveNumber][0]; }

	void increaseWaveNumber() { m_currentWaveNumber++; }





private:

	int m_numWaves = 0;
	int m_currentWaveNumber = 1;
	std::vector< std::vector < int > > m_waveData;



	int m_numInputs = 0;
	int m_numRows = 0;
	int m_numColumns = 0;


};

