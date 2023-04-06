#include "AIController.h"
#include "Timer.h"
#include "GameState.h"
#include <iostream>
#include <Windows.h>

#include "GameController.h"

using namespace std;

/*
* DNA = a list of towers, each with a position
* 
	1) generate population: a list of DNA
	2) evaluate population: run the game with each DNA
	3) select fittest: select the best DNA
*/

AIController::AIController()
{
	m_gameController = nullptr;
	m_gameBoard = nullptr;
	m_Timer = nullptr;
	m_gameState = nullptr;

	srand(NUM_TOWERS);
	CreatePopulation(NUM_TOWERS);
}

AIController::~AIController()
{

}

void AIController::gameOver()
{
}

void AIController::update()
{
	if (m_Timer == nullptr)
		return;

	DNA2* dna = new DNA2();

	// HINT
	// a second has elapsed - your GA manager (GA Code) may decide to do something at this time...
	static double elapsedSeconds = 0;
	double seconds = floor(m_Timer->elapsedSeconds());
	if (seconds > elapsedSeconds)
	{
		elapsedSeconds = seconds;

	}

	if (elapsedSeconds >= dna->m_spawnDelay)
	{
		bool wasTowerAdded = addTower(dna->m_towerType, dna->m_towerPosition.x, dna->m_towerPosition.y);
		dna->m_score = wasTowerAdded ? 1 : -10;

	}
	//GAManager::Instance()->Update(m_Timer->elapsedSeconds());

	// this might be useful? Monsters killed
	static int monstersKilled = 0;

	if (m_gameState->getMonsterEliminated() > monstersKilled)
	{
		monstersKilled = m_gameState->getMonsterEliminated();
	}

	recordScore();
}

bool AIController::addTower(TowerType type, int gridx, int gridy)
{
	// grid position can be from 0,0 to 25,17
	/*
	enum class TowerType {
	empty, slammer, swinger, thrower };
	*/

	return m_gameBoard->addTower(type, gridx, gridy);
	
	//bool towerAdded = m_gameBoard->addTower(type, gridx, gridy);

	// NOTE towerAdded might be false if the tower can't be placed in that position, is there isn't enough funds
	/* if tower is not added, 
		a) record the position
		b) record the type
		c)
	*/ 
}

void AIController::setupBoard()
{
	m_Timer->start();
}

int AIController::recordScore()
{
	int currentWave = m_gameState->getCurrentWave();
	int killCount = m_gameState->getMonsterEliminated();
	currentWave *= 10; // living longer is good
	int score = currentWave + killCount;

	static int iteration = 0;

	if (iteration == 0)
		cout << "iteration" << "," << "wave" << "," << "kills" << "," << "score" << endl;

	cout << iteration << "," << m_gameState->getCurrentWave() << "," << m_gameState->getMonsterEliminated() << "," << score << endl;
	iteration++;

	m_gameState->setScore(score);

	return score;
}

void AIController::CreatePopulation(int numTowers)
{
	for (int i = 0; i < numTowers; i++)	
	{
		m_population.push_back(CreateDNA());
		//m_towerTypes.push_back((TowerType)(rand() % 3 + 1));
		//m_towerPositions.push_back(sf::Vector2f(rand() % MAX_COL, rand() % MAX_ROW));
	}
}

DNA AIController::CreateDNA()
{
	DNA dna;
	for (int i = 0; i < NUM_TOWERS; i++)
	{
		dna.m_towerTypes.push_back((TowerType)(rand() % 3 + 1));
		dna.m_towerPositions.push_back(sf::Vector2f(rand() % MAX_COL, rand() % MAX_ROW));
	}
	dna.m_score = 0;
	return dna;
}