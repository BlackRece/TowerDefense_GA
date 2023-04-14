#include "DNA.h"

DNA::DNA() : m_score(0), m_kills(0), m_duration(0), m_index(0)
{
}

DNA::~DNA()
{
	for (int i = 0; i < m_genes.size(); i++)
		delete m_genes[i];
	m_genes.clear();
}

DNA* DNA::copy()
{
	DNA* dna = new DNA();
	dna->m_score = m_score;
	dna->m_duration = m_duration;
	dna->m_kills = m_kills;
	dna->m_genes = m_genes;
	return dna;
}

DNA* DNA::reverse()
{
	DNA* dna = new DNA();

	for (int i = m_genes.size() - 1; i >= 0 ; i--)
		dna->m_genes.push_back(m_genes[i]);
	
	return dna;
}

DNA* DNA::setHigh() {
	DNA* dna = new DNA();
	dna->m_score = RAND_MAX;
	dna->m_duration = RAND_MAX;
	dna->m_kills = RAND_MAX;
	return dna;
}

std::tuple<DNA*, DNA*> DNA::crossover(DNA& dnaA, DNA& dnaB, int step)
{
	/*if (dnaA.m_genes.size() < GENES_PER_DNA)
	{
		std::string msg = 
			"DNA::crossover() - dnaA has less than " 
			+ std::to_string(GENES_PER_DNA)
			+ " genes.";
		throw std::exception(msg.c_str());
	}
	
	if (dnaB.m_genes.size() < GENES_PER_DNA)
	{
		std::string msg =
			"DNA::crossover() - dnaB has less than "
			+ std::to_string(GENES_PER_DNA)
			+ " genes.";
		throw std::exception(msg.c_str());
	}*/
		
	DNA* dna1 = new DNA();
	DNA* dna2 = new DNA();

	for (int i = 0; i < GENES_PER_DNA; i++)
	{
		bool bFlip = false;
		if (i % step == 0)
			bFlip = !bFlip;

		if (bFlip)
		{
			dna1->m_genes.push_back(
				dnaA.m_genes.size() < i + 1
				? new Gene()
				: dnaA.m_genes[i]
			);
			dna2->m_genes.push_back(
				dnaB.m_genes.size() < i + 1
				? new Gene()
				: dnaB.m_genes[i]
			);
		}
		else
		{
			dna1->m_genes.push_back(
				dnaB.m_genes.size() < i + 1
				? new Gene()
				: dnaB.m_genes[i]
			);
			dna2->m_genes.push_back(
				dnaA.m_genes.size() < i + 1
				? new Gene()
				: dnaA.m_genes[i]
			);
		}
	}

	return std::make_tuple(dna1, dna2);
}

bool DNA::isGreaterThan(DNA& dna)
{
	if (m_score == dna.m_score)
	{
		if (m_kills == dna.m_kills)
			return m_duration > dna.m_duration;
		else
			return m_kills > dna.m_kills;
	}
	return m_score > dna.m_score;
}

bool DNA::isEqualTo(DNA& dna)
{
	if (m_genes.size() != dna.m_genes.size())
		return false;

	for (int i = 0; i < m_genes.size(); i++)
	{
		if (m_genes[i]->m_towerType != dna.m_genes[i]->m_towerType)
			return false;
		if (m_genes[i]->m_towerPosition != dna.m_genes[i]->m_towerPosition)
			return false;
	}

	return m_score == dna.m_score && m_kills == dna.m_kills && m_duration == dna.m_duration;
}

void DNA::fillGenes()
{
	for (int i = 0; i < GENES_PER_DNA; i++)
		m_genes.push_back(getUniqueGene());
}

bool DNA::nextGene()
{
	m_index++;
	if (m_index >= m_genes.size())
	{
		m_index = 0;
		return true;
	}
	
	return false;
}

void DNA::mutateGene(float fPercent)
{
	for (Gene* gene : m_genes)
	{
		if ((rand() % 100) <= fPercent)
			gene = getUniqueGene();
	}
}

DNAJson DNA::getJson()
{
	DNAJson json;

	json.m_score = m_score;
	json.m_kills = m_kills;
	json.m_duration = m_duration;

	for (Gene* gene : m_genes)
		json.m_genes.push_back(gene->getJson());

	return json;
}

void DNA::setJson(DNAJson& json)
{
	m_score = json.m_score;
	m_kills = json.m_kills;
	m_duration = json.m_duration;
	for (GeneJson geneJson : json.m_genes)
	{
		Gene* gene = new Gene();
		gene->setJson(geneJson);
		m_genes.push_back(gene);
	}
}

Gene* DNA::getUniqueGene()
{
	Gene* newGene = new Gene();
	bool isUnique = false;

	while (isUnique == false)
	{
		// set flag
		isUnique = true;

		// for each gene
		for (Gene* gene : m_genes)
		{
			// check for dupicates
			if (gene->m_towerPosition == newGene->m_towerPosition)
			{
				// if duplicated, set flag
				isUnique = false;
				break;
			}
		}

		// if duplicated flag was set, get new position
		if (!isUnique)
			newGene = new Gene();
	}

	return newGene;
}

void DNA::DebugReport()
{
	std::cout
		<< "\n\tScore: " << m_score
		<< "\n\tKills: " << m_kills
		<< "\n\tDuration: " << m_duration;
}