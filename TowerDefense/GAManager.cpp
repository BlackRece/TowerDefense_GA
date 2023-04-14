#include "GAManager.h"

GAManager::GAManager() : m_index(0), m_generation(0), m_isWaitingForNewGame(false)
{
	// seed random number generator with current time
	std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
	std::chrono::system_clock::duration dtn = tp.time_since_epoch();
	srand(dtn.count());

	// load population from json file
	loadChromosFromJson();

	// if no population was loaded, create initial population
	if (m_chromos.size() < 1)
		// create initial population
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

	if (USE_ELITE)
	{
		// Selection: select the best chromosomes (aka Elite/Ranked Selection)
		std::tuple<DNA*, std::vector<DNA*>> selectionResult = selection_elite();
		DNA* eliteDNA = std::get<0>(selectionResult);
		std::vector<DNA*> vecSelected = std::get<1>(selectionResult);

		// Mating Pool: create chromosome pool from selected chromosomes
		// std::vector<DNA*> vecMatingPool = matingPool(vecSelected);
		std::vector<DNA*> vecMatingPool = fastMatingPool(vecSelected);

		// Crossover: create new chromosomes from selected chromosomes
		std::vector<DNA*> vecCrossed = crossoverWithParent(*eliteDNA, vecMatingPool, CROSSOVER_STEP);

		// Mutation: mutate crossed chromosomes using a mutation percentage
		m_chromos = mutation(vecCrossed, MUTATION_PERCENTAGE);

		// add elite dna 
		m_chromos.push_back(eliteDNA);
		m_chromos.push_back(eliteDNA->reverse());
	}
	else
	{
		// Selection: select the best chromosomes (aka Elite/Ranked Selection)
		std::vector<DNA*> vecSelected = selection_rank();
		
		// Mating Pool: create chromosome pool from selected chromosomes
		// std::vector<DNA*> vecMatingPool = matingPool(vecSelected);
		std::vector<DNA*> vecMatingPool = fastMatingPool(vecSelected);

		// Crossover: create new chromosomes from selected chromosomes
		std::vector<DNA*> vecCrossed = crossover(vecMatingPool, CROSSOVER_STEP);

		// Mutation: mutate crossed chromosomes using a mutation percentage
		m_chromos = mutation(vecCrossed, MUTATION_PERCENTAGE);
	}

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

int GAManager::findDNAIndex(DNA& target, std::vector<DNA*> vecSource) {
	for (int i = 0; i < vecSource.size(); i++)
	{
		if (target.isEqualTo(*vecSource[i]))
			return i;
	}
	return -1;
}

DNA* GAManager::findBestDNA(std::vector<DNA*> vecSource)
{
	DNA* bestDNA = new DNA();

	for (DNA* dna : vecSource)
	{
		// if copy dna with the higher fitness score
		if (bestDNA->isLesserThan(*dna) && !bestDNA->isEqualTo(*dna))
			bestDNA = dna->copy();
	}

	return bestDNA;
}

bool GAManager::isUnique(DNA& target, std::vector<DNA*>& vecSource)
{
	// if target is in list, the not unique
	for (DNA* dna : vecSource)
	{
		if (target.isEqualTo(*dna))
			return false;
	}

	return true;
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

std::tuple<DNA*, std::vector<DNA*>> GAManager::selection_elite()
{
	// copy chromos list 
	std::vector<DNA*> vecCopiedChromos = m_chromos;

	// select ELITE dna from copied chromos list
	DNA* eliteDNA = findBestDNA(m_chromos);

	// remove best dna from copied chromos list
	int bestIndex = findDNAIndex(*eliteDNA, vecCopiedChromos);
	vecCopiedChromos.erase(vecCopiedChromos.begin() + bestIndex);

	// split copied chromos into 3 groups
	std::vector<DNA*> vecFirst, vecSecond, vecThird;
	/*for (int i = 0; i < vecCopiedChromos.size() - 1; i += 3)
	{
		vecFirst.push_back(vecCopiedChromos[i]);
		vecSecond.push_back(vecCopiedChromos[i + 1]);
		vecThird.push_back(vecCopiedChromos[i + 2]);
	}*/
	for (DNA* dna : vecCopiedChromos)
	{
		if (vecFirst.size() <= 3)
			vecFirst.push_back(dna);
		else if (vecSecond.size() <= 3)
			vecSecond.push_back(dna);
		else if (vecThird.size() <= 3)
			vecThird.push_back(dna);
	}

	// select the best from each grouping
	DNA* firstDNA = findBestDNA(vecFirst);
	DNA* secondDNA = findBestDNA(vecSecond);
	DNA* thirdDNA = findBestDNA(vecThird);

	// store selected DNA into single array
	std::vector<DNA*> vecBestChromos;
	//vecBestChromos.push_back(eliteDNA);
	vecBestChromos.push_back(eliteDNA->reverse());
	vecBestChromos.push_back(firstDNA);
	vecBestChromos.push_back(secondDNA);
	vecBestChromos.push_back(thirdDNA);

	// return result
	return std::make_tuple(eliteDNA, vecBestChromos);
}

std::vector<DNA*> GAManager::selection_rank()
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
		DNA* dnaA = vecMatingPool[i];
		DNA* dnaB = vecMatingPool[i+1];

		std::tuple<DNA*, DNA*> offspring = DNA::crossover(*dnaA, *dnaB, step);
		vecOffSpring.push_back(std::get<0>(offspring));
		vecOffSpring.push_back(std::get<1>(offspring));
	}

	return vecOffSpring;
}

std::vector<DNA*> GAManager::crossoverWithParent(DNA& parent, std::vector<DNA*>& vecMatingPool, int step)
{
	std::vector<DNA*> vecOffSpring;

	for (int i = 0; i < floor(DNA_PER_CHROMOSOME / 2); i++)
	{
		DNA* dna = vecMatingPool[i];

		std::tuple<DNA*, DNA*> offspring = DNA::crossover(parent, *dna, step);
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

void GAManager::loadChromosFromJson()
{
	// load chromos from json file
	std::vector<DNAJson> vecChromosJson;
	JsonParser::LoadJson(vecChromosJson, STORED_DNA_FILE_NAME);

	// convert json to dna
	std::vector<DNA*> vecChromos;
	for (DNAJson dnaJson : vecChromosJson)
	{
		DNA* dna = new DNA();
		dna->setJson(dnaJson);
		vecChromos.push_back(dna);
	}

	// set chromos
	m_chromos = vecChromos;
}

void GAManager::saveChromosToJson()
{
	// generate vector of chromos to save as json
	std::vector<DNAJson> vecChromosJson;
	for (DNA* dna : m_chromos)
	{
		DNAJson dnaJson = dna->getJson();
		vecChromosJson.push_back(dnaJson);
	}

	// build file name from generation count
	std::string fileName = FILE_NAME + std::to_string(m_generation) + FILE_EXT;

	// save chromos to json file
	JsonParser::SaveJson(vecChromosJson, fileName);
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

	// save chromos to json file
	saveChromosToJson();
}