#pragma once
#ifndef DNA_H
#define DNA_H

#include "Gene.h"
#include <vector>
#include <string>

constexpr int GENES_PER_DNA = 40;

class DNA
{
public:
	DNA();
	~DNA();
	
	static DNA* setHigh();
	static std::tuple<DNA*, DNA*> crossover(DNA& dnaA, DNA& dnaB, int step);

	float fitness() 
	{
		float score = m_score - 40;
		float fraction = score / 200;
		float percent = fraction * 100;
		return percent;
	}
	inline void resetIndex() { m_index = 0; }

	DNA* copy();
	DNA* reverse();
	bool isGreaterThan(DNA& dna);
	inline bool isLesserThan(DNA& dna) { return !isGreaterThan(dna); }
	bool isEqualTo(DNA& dna);
	
	void fillGenes();
	inline Gene& getGene() { return *m_genes[m_index]; }
	//inline Gene& getRandomGene() { return *m_genes[rand() % m_genes.size()]; }
	bool nextGene();
	void mutateGene(float fPercent);
	Gene* getUniqueGene();
	
	void DebugReport();
	
	std::vector<Gene*> m_genes;
	int m_score;
	int m_kills;
	int m_duration;

private:
	int m_index;
};

#endif // !DNA_H