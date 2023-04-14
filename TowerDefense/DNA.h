#pragma once
#ifndef DNA_H
#define DNA_H

#include "Gene.h"
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

constexpr int GENES_PER_DNA = 40;

struct DNAJson
{
	std::vector<GeneJson> m_genes;
	int m_score;
	int m_kills;
	int m_duration;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(DNAJson, m_genes, m_score, m_kills, m_duration);
};

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
	bool nextGene();
	void mutateGene(float fPercent);
	Gene* getUniqueGene();
	
	DNAJson getJson();
	void setJson(DNAJson& json);

	void DebugReport();
	
	std::vector<Gene*> m_genes;
	int m_score;
	int m_kills;
	int m_duration;

private:
	int m_index;
};

#endif // !DNA_H