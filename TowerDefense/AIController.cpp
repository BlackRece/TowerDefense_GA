#include "AIController.h"
#include "Timer.h"
#include "GameState.h"
#include <iostream>
#include <Windows.h>

#include "GameController.h"

using namespace std;

/*
* Population -> Chromosomes -> DNA -> Genes 
* A Population has a group of Chromosomes,
* A Chromosome has a group of DNA
* A DNA has a score, a kills counter and a group of Genes
* A Gene has a tower, a position and a spawn delay
* 
	1) generate chromosome: a list of DNA
	2) evaluate chromosome: run the game with each DNA
	3) select fittest: select the best DNA
*/

AIController::AIController()
{
	m_gameController = nullptr;
	m_gameBoard = nullptr;
	m_Timer = nullptr;
	m_gameState = nullptr;

	m_DNAIndex = 0;
	m_chromoIndex = 0;
	m_geneIndex = 0;

	srand(DNA_PER_CHROMOSOME * GENES_PER_DNA * MAX_COL * MAX_ROW * SPAWN_DELAY);
	CreatePopulation();
}

AIController::~AIController()
{

}

void AIController::gameOver()
{
	// update DNA
	DNA dna = m_chromos[m_chromoIndex];
	dna.m_score = recordScore();
	dna.m_kills = m_gameState->getMonsterEliminated();
	dna.m_duration = floor(m_Timer->elapsedSeconds());
	
	// select next chromosome
	if(m_chromoIndex < m_chromos.size())
		m_chromoIndex++;
}

void AIController::update()
{
	if (m_Timer == nullptr)
		return;

	// HINT
	// a second has elapsed - your GA manager (GA Code) may decide to do something at this time...
	static double elapsedSeconds = 0;
	double seconds = floor(m_Timer->elapsedSeconds());
	if (seconds > elapsedSeconds)
	{
		elapsedSeconds = seconds;

	}

	Gene* gene = m_chromos[m_chromoIndex].m_genes[m_DNAIndex];
	if (elapsedSeconds >= gene->m_spawnDelay)
	{
		bool wasTowerAdded = addTower(gene->m_towerType, gene->m_towerPosition.x, gene->m_towerPosition.y);
		gene->m_score = wasTowerAdded ? 1 : -10;
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

void AIController::CreatePopulation()
{
	for (int j = 0; j < DNA_PER_CHROMOSOME; j++)
	{ 
		DNA dna;
		dna.m_kills = 0;
		dna.m_score = 0;
		
		for (int i = 0; i < GENES_PER_DNA; i++)	
		{
			dna.m_genes.push_back(new Gene());
		}
		
		m_chromos.push_back(dna);
	}
}
