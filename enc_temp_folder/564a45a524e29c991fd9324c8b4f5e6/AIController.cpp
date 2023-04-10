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
	m_generation = 0;

	randomSeed(DNA_PER_CHROMOSOME * GENES_PER_DNA * MAX_COL * MAX_ROW * SPAWN_DELAY);
	m_chromos = CreatePopulation(DNA_PER_CHROMOSOME);
}

AIController::~AIController()
{

}

void AIController::gameOver()
{
	
	if (m_chromoIndex < m_chromos.size())
	{
		// update DNA
		DNA* dna = m_chromos[m_chromoIndex];
		dna->m_kills = m_gameState->getMonsterEliminated();
		dna->m_duration = floor(m_Timer->elapsedSeconds());
		dna->m_score = recordScore();
	
		// select next chromosome
		m_chromoIndex++;

		// reset dna index
		m_DNAIndex = 0;

		return;
	}

	cout
		<< "\n<-------------->"
		<< "\nREPORT: Generation " << m_generation;

	for (int i = 0; i < m_chromos.size(); i++)
	{
		// DEBUG: report
		DNA* dna = m_chromos[i];
		if (dna->m_score > 0)
		{
			cout
				<< "\n<-------------->"
				<< "\nChromosome: " << i;
			dna->DebugReport();
		}
	}

	// Selection: select the best chromosomes
	int iBestCount = (int)DNA_PER_CHROMOSOME / 2;
	std::vector<DNA*> vecParentChromos;
	for (int i = 0; i < iBestCount; i++)
	{
		DNA* bestDNA = new DNA();

		for (DNA* dna : m_chromos)
		{
			// if already in parent list, skip
			bool found = false;

			for (DNA* foundDna : vecParentChromos)
			{
				if (dna->isEqual(foundDna))
				{
					found = true;
					break;
				}
			}

			if (found)
				continue;

			// if better than current dna, copy
			if (bestDNA->isLesser(dna) && !bestDNA->isEqual(dna))
				bestDNA = dna->copy();
		}

		vecParentChromos.push_back(bestDNA);
	}

	cout << "Best " << vecParentChromos.size() << " Chromosomes selected." << endl;

	// Crossover: mating pool comprises of vecBestChromos
	// next step is to produce the next generation of chromosomes.
	/*
	* 5 = 10
	* 4 = 8
	* 3 = 6
	* 2 = 4
	* 1 = 2
	* t = 30
	*/
	// step 1 - create mating pool
	std::vector<DNA*> vecMatingChromos;
	for (DNA* dna : vecParentChromos)
	{
		int iCount = (dna->fitness() / 100.0f) * 10;

		for (int j = 0; j < iCount; j++)
		{
			DNA* dnaCopy = dna->copy();
			vecMatingChromos.push_back(dnaCopy);
		}
	}

	// step 2 - create next generation
	std::vector<DNA*> vecOffspringChromos;
	int iMatingCount = vecMatingChromos.size();
	for (int i = 0; i < DNA_PER_CHROMOSOME / 2; i++)
	{
		DNA& dnaA = *vecMatingChromos[rand() % iMatingCount];
		DNA& dnaB = *vecMatingChromos[rand() % iMatingCount];

		auto dnaPair = DNA::crossover(dnaA, dnaB, 2);
		vecOffspringChromos.push_back(std::get<0>(dnaPair));
		vecOffspringChromos.push_back(std::get<1>(dnaPair));
	}
	
	// Mutation: mutate the offspring chromosomes
	for(DNA* rawDNA: vecOffspringChromos)
	{
		//float fMutationPercentage = (rawDNA->m_score / 200.0f) * 100;
		float fMutationPercentage = 5.0f;

		for(Gene* rawGene : rawDNA->m_genes)
		{
			if ((rand() % 100) < fMutationPercentage /*|| !rawGene->IsPlaced()*/)
				rawGene = new Gene();
		}
	}

	// merge partent and offspring chromos
	m_chromos = vecOffspringChromos;

	// reset chromosome index
	m_chromoIndex = 0;
	
	// reset dna index
	m_DNAIndex = 0;
	
	// reset gene index
	m_geneIndex = 0;
	
	// new generation
	m_generation++;
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

	if (m_chromoIndex < m_chromos.size())
	{
		DNA* dna = m_chromos[m_chromoIndex];
		if (m_DNAIndex < dna->m_genes.size())
		{
			Gene* gene = dna->m_genes[m_DNAIndex];

			// check if funds are available
			if (m_gameState->getTams() >= m_gameState->getTowerProps(gene->m_towerType)["tam"])
			{
				if (addTower(gene->m_towerType, gene->m_towerPosition.x, gene->m_towerPosition.y))
					gene->MarkAsPlaced();

				if (m_DNAIndex < dna->m_genes.size())
					m_DNAIndex++;
			}
		}
	}
	else
		return;
	
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

	/*static int iteration = 0;

	if (iteration == 0)
		cout 
			<< "iteration" << "," 
			<< "wave" << "," 
			<< "kills" << "," 
			<< "score" << endl;

	cout 
		<< iteration << "," 
		<< m_gameState->getCurrentWave() << "," 
		<< m_gameState->getMonsterEliminated() << "," 
		<< score << endl;

	iteration++;*/

	m_gameState->setScore(score);

	return score;
}

void AIController::CreatePopulation()
{
	for (int j = 0; j < DNA_PER_CHROMOSOME; j++)
	{ 
		DNA* dna = new DNA();
		dna->m_kills = 0;
		dna->m_score = 0;
		
		for (int i = 0; i < GENES_PER_DNA; i++)	
			dna->m_genes.push_back(new Gene());
		
		m_chromos.push_back(dna);
	}
}

std::vector<DNA*> AIController::CreatePopulation(int dnaCount)
{
	std::vector<DNA*> vecChromos;
	for (int j = 0; j < dnaCount; j++)
	{
		DNA* dna = new DNA();
		dna->m_kills = 0;
		dna->m_score = 0;

		for (int i = 0; i < GENES_PER_DNA; i++)
			dna->m_genes.push_back(new Gene());

		vecChromos.push_back(dna);
	}
	return vecChromos;
}

void AIController::randomSeed(int seed)
{
	int a = rand() % seed;
	int b = rand() % seed;
	int c = rand() % seed;

	int rndSeed = (int)((a + b + c) / 3);
	srand(rndSeed);
}