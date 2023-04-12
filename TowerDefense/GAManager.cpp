#include "GAManager.h"

GAManager::GAManager() : m_index(0), m_generation(0), m_isWaitingForNewGame(false)
{
	// seed random number generator with current time
	std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
	std::chrono::system_clock::duration dtn = tp.time_since_epoch();
	srand(dtn.count());

	createPopulation(DNA_PER_CHROMOSOME);
}

void GAManager::markGene()
{
	Gene& gene = getGene();
	markAsPlaced(gene.getPosition());
}

void GAManager::nextGene()
{
	if (m_isWaitingForNewGame)
		return;

	if (!m_chromos[m_index]->nextGene())
		return;

	m_isWaitingForNewGame = true;
}

void GAManager::nextDNA()
{

	if (m_index + 1 >= m_chromos.size())
	{
		nextGeneration();
		return;
	}

	m_index++;
	m_chromos[m_index]->resetIndex();
}

void GAManager::storeFitnessData(int iScore, int iKills, int iDuration)
{
	m_chromos[m_index]->m_score = iScore;
	m_chromos[m_index]->m_kills = iKills;
	m_chromos[m_index]->m_duration = iDuration;

	m_isWaitingForNewGame = false;

	nextDNA();
}

void GAManager::nextGeneration()
{
	DebugReport();

	// Selection: select the best chromosomes (aka Elite/Ranked Selection)
	std::vector<DNA*> vecSelected = selection();
	
	// Mating Pool: create chromosome pool from selected chromosomes
	//std::vector<DNA*> vecMatingPool = matingPool(vecSelected);
	std::vector<DNA*> vecMatingPool = fastMatingPool(vecSelected);
	
	// Crossover: create new chromosomes from selected chromosomes
	std::vector<DNA*> vecCrossed = crossover(vecMatingPool, CROSSOVER_STEP);

	// Mutation: mutate crossed chromosomes using a mutation percentage
	m_chromos = mutation(vecCrossed, MUTATION_PERCENTAGE);

	// Reset: prepare for next generation
	reset();

}

void GAManager::createPopulation(int dnaCount)
{
	for (int i = 0; i < dnaCount; i++)
	{
		DNA* dna = new DNA();
		dna->fillGenes();

		m_chromos.push_back(dna);
	}
}

int GAManager::getRandomIndex(std::vector<DNA*>& vecSource)
{
	float iChance = rand() % 100;
	for (int i = 0; i < vecSource.size(); i++)
	{
		float fFitness = vecSource[i]->fitness();
		iChance -= fFitness;
		if (iChance <= 0)
			return i;
	}

	return 0;
}

int GAManager::findPlacedIndex(sf::Vector2f pos)
{
	for (int i = 0; i < m_vecPlaced.size(); i++)
	{
		if (m_vecPlaced[i] == pos)
			return i;
	}

	return -1;
}

std::vector<DNA*> GAManager::matingPool(std::vector<DNA*>& vecSelected)
{
	std::vector<DNA*> vecMatingPool;
	for (DNA* dna : vecSelected)
	{
		float fFitnessMultiplier = (dna->fitness() / 100.0f);
		int iMatingPoolCount = floor(DNA_PER_CHROMOSOME / 2);

		// calculate how many times to add dna to mating pool
		int iCount = floor(fFitnessMultiplier * iMatingPoolCount);

		for (int j = 0; j < iCount; j++)
		{
			DNA* dnaCopy = dna->copy();
			vecMatingPool.push_back(dnaCopy);
		}
	}
	return vecMatingPool;
}

std::vector<DNA*> GAManager::fastMatingPool(std::vector<DNA*>& vecSelected)
{
	std::vector<DNA*> vecMatingPool;
	
	for (int i = 0; i < DNA_PER_CHROMOSOME; i++)
		vecMatingPool.push_back(vecSelected[getRandomIndex(vecSelected)]);
	
	return vecMatingPool;
}

std::vector<DNA*> GAManager::selection()
{
	// no. of best chromosomes to select
	int iBestCount = DNA_PER_CHROMOSOME / 2;
	
	std::vector<DNA*> vecBestChromos;
	for (int i = 0; i < iBestCount; i++)
	{
		DNA* bestDNA = new DNA();

		for (DNA* dna : m_chromos)
		{
			// if already in parent list, skip
			bool found = false;

			for (DNA* bestDna : vecBestChromos)
			{
				if (dna->isEqualTo(*bestDna))
				{
					found = true;
					break;
				}
			}

			if (found)
				continue;

			// if copy dna with the higher fitness score
			if (bestDNA->isLesserThan(*dna) && !bestDNA->isEqualTo(*dna))
				bestDNA = dna->copy();
		}

		vecBestChromos.push_back(bestDNA);
	}
	
	return vecBestChromos;
}

std::vector<DNA*> GAManager::crossover(std::vector<DNA*>& vecMatingPool, int step)
{
	std::vector<DNA*> vecOffSpring;
	int iMatingCount = vecMatingPool.size();
	
	for (int i = 0; i < floor(iMatingCount / 2); i++)
	{
		DNA* dnaA = vecMatingPool[rand() % iMatingCount];
		DNA* dnaB = vecMatingPool[rand() % iMatingCount];

		std::tuple<DNA*, DNA*> offspring = DNA::crossover(*dnaA, *dnaB, step);
		vecOffSpring.push_back(std::get<0>(offspring));
		vecOffSpring.push_back(std::get<1>(offspring));
	}

	return vecOffSpring;
}

std::vector<DNA*> GAManager::mutation(std::vector<DNA*>& vecOffSpring, float fMutationPercentage)
{
	for (DNA* dna : vecOffSpring)
		dna->mutateGene(fMutationPercentage);
	
	return vecOffSpring;
}

void GAManager::reset()
{
	m_index = 0;
	for (DNA* dna : m_chromos)
		dna->resetIndex();
	
	// increase generation count
	m_generation++;

	m_isWaitingForNewGame = false;
}

void GAManager::DebugReport()
{
	cout
		<< "\n<-------------->"
		<< "\nREPORT: Generation " << m_generation;
	
	for (int i = 0; i < m_chromos.size(); i++)
	{
		cout << "\n\nChromosome: " << i;
		m_chromos[i]->DebugReport();
	}
}