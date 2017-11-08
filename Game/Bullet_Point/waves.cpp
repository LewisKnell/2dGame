
#include <SDL\SDL.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <random>
#include <ctime>

#include <LKengine\myErrors.h>

#include "zombie.h"
#include "waves.h"

Waves::Waves()
{
	// Empty
}


Waves::~Waves()
{
	// Empty
}


void Waves::init(const std::string& fileName)
{
	std::ifstream file;
	file.open(fileName);

	//error checking
	if (file.fail())
	{
		LKengine::fatalError("Failed to open " + fileName);
	}

	bool countColumns = true;
	std::string line;
	int isTrue = true;

	// Go throught getting each line of the input file
	while (std::getline(file, line))
	{
		std::vector<int> row;
		std::stringstream iss(line);
		int c = 0;
		std::string val;

		m_numRows++;
		// Add a wave for each row of the file
		m_numWaves++;

		// Go along the row and take the value up to the "," then put that into the vector as the value
		while (getline(iss, val, ','))
		{
			// no need for converting as you are reading string.
			row.push_back(std::stoi(val));
			m_numInputs++;

			if (countColumns == true)
				m_numColumns++;
		}
		m_waveData.push_back(row);
		countColumns = false;
	}


	// Debug output
	std::cout << "Wave Data loaded \n";
	//std::cout << m_numInputs << " Inputs \n";
	//std::cout << m_numRows << " Rows \n";
	//std::cout << m_numColumns << " Columns \n";
	std::cout << "There are " << m_numWaves << " waves in this level \n";


	//print it out how many enemies in each wave
	for (int i = 0; i < m_waveData.size(); i++)
	{
		for (int j = 0; j < m_waveData[0].size(); j++)
		{
			std::cout << "Wave " << i << " has: " << m_waveData[i][j] << " enemies " << std::endl;
		}
	}


	m_currentWaveNumber = 1;

}


void Waves::generateWave(const std::vector<glm::vec2>& zombieStartPosition, std::vector<Zombie*>& m_zombies)
{
	//add the zombies
	static std::mt19937 randomEngine(time(nullptr));
	//get random direction using the _spread (accuracy) variable
	std::uniform_real_distribution<float> randOffset(1, 10);

	for (int i = 0; i < m_waveData[m_currentWaveNumber-1][0]; i++)
	{
			m_zombies.push_back(new Zombie);
			m_zombies.back()->init(ZOMBIE_SPEED, zombieStartPosition[0] + randOffset(randomEngine));
	}

}
