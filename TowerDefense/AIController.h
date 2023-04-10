#pragma once

#include "Tower.h"
#include <vector>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#define DNA_PER_CHROMOSOME 10
#define GENES_PER_DNA 10
#define MAX_COL 25
#define MAX_ROW 17
#define SPAWN_DELAY 10

class GameController;
class GameBoard;
class Timer;
class GameState;

class Gene
{
public:
	Gene() 
	{
		m_towerType = (TowerType)(rand() % 3 + 1);
		m_towerPosition = sf::Vector2f(rand() % MAX_COL, rand() % MAX_ROW);
		m_isPlaced = false;
	}
	~Gene() {}

	inline bool IsPlaced() { return m_isPlaced; }
	inline void MarkAsPlaced() { m_isPlaced = true; }

	void copy(Gene* gene)
	{
		m_towerType = gene->m_towerType;
		m_towerPosition = gene->m_towerPosition;
		m_isPlaced = gene->m_isPlaced;
	}

	TowerType m_towerType;
	sf::Vector2f m_towerPosition;

private:
	bool m_isPlaced;
};

class DNA
{
public:
	DNA() : m_score(0), m_kills(0), m_duration(0) {}
	~DNA()
	{
		for (int i = 0; i < m_genes.size(); i++)
			delete m_genes[i];
		m_genes.clear();
	}

	std::vector<Gene*> m_genes;
	int m_score;
	int m_kills;
	int m_duration;

	/*inline */float fitness() 
	{ 
		float val = m_score / 200.0f;
		return val * 100;
	}

	DNA* copy()
	{
		DNA* dna = new DNA();
		dna->m_score = m_score;
		dna->m_kills = m_kills;
		dna->m_genes = m_genes;
		return dna;
	}

	static std::tuple<DNA*, DNA*> crossover(DNA& dnaA, DNA& dnaB, int step)
	{
		DNA* dna1 = new DNA();
		DNA* dna2 = new DNA();

		for (int i = 0; i < GENES_PER_DNA; i++)
		{
			bool bFlip = false;
			if (i % step == 0)
				bFlip = !bFlip;

			if (bFlip)
			{
				dna1->m_genes.push_back(dnaA.m_genes[i]);
				dna2->m_genes.push_back(dnaB.m_genes[i]);
			}
			else
			{
				dna1->m_genes.push_back(dnaB.m_genes[i]);
				dna2->m_genes.push_back(dnaA.m_genes[i]);
			}
		}

		return std::make_tuple(dna1, dna2);
	}

	static DNA* setHigh() {
		DNA* dna = new DNA();
		dna->m_score = 200;
		dna->m_kills = 200;
		return dna;
	}

	bool isGreater(DNA* dna)
	{
		if (m_score == dna->m_score)
			return m_kills > dna->m_kills;
		return m_score > dna->m_score;
	}

	bool isLesser(DNA* dna)
	{
		return !isGreater(dna);
	}

	bool isEqual(DNA* dna)
	{	
		if (dna == nullptr)
			return false;

		if (m_genes.size() != dna->m_genes.size())
			return false;

		for (int i = 0; i < GENES_PER_DNA; i++)
		{
			if (m_genes[i]->m_towerType != dna->m_genes[i]->m_towerType)
				return false;
			if (m_genes[i]->m_towerPosition != dna->m_genes[i]->m_towerPosition)
				return false;
		}

		return m_score == dna->m_score && m_kills == dna->m_kills;
	}

	friend bool operator > (DNA& lDna, DNA& rDna)
	{
		if (lDna.m_score > rDna.m_score) return true;
		if (lDna.m_score == rDna.m_score)
			return lDna.m_kills > rDna.m_kills;
	}

	friend bool operator < (DNA& lDna, DNA& rDna)
	{
		if (lDna.m_score < rDna.m_score) return true;
		if (lDna.m_score == rDna.m_score)
			return lDna.m_kills < rDna.m_kills;
	}
	
	friend bool operator == (DNA& lDna, DNA& rDna)
	{
		for (int i = 0; i < GENES_PER_DNA; i++)
		{
			if (lDna.m_genes[i]->m_towerType != rDna.m_genes[i]->m_towerType)
				return false;
			if (lDna.m_genes[i]->m_towerPosition != rDna.m_genes[i]->m_towerPosition)
				return false;
		}
		return lDna.m_score == rDna.m_score && lDna.m_kills == rDna.m_kills;
	}

	void DebugReport()
	{
		std::cout
			<< "\n\tScore: " << m_score
			<< "\n\tKills: " << m_kills
			<< "\n\tDuration: " << m_duration;
	}
};

class AIController
{
public:
	AIController();
	~AIController();

	void setupBoard();
	int  recordScore();

	void setGameController(GameController* gameController) { m_gameController = gameController; }
	void setGameBoard(GameBoard* gameBoard) { m_gameBoard = gameBoard; }
	void setTimer(Timer* timer) { m_Timer = timer; }
	void setGameState(GameState* gameState) { m_gameState = gameState; }
	void update();
	bool addTower(TowerType type, int gridx, int gridy);
	void gameOver();

private:
	void CreatePopulation();
	std::vector<DNA*> CreatePopulation(int dnaCount);
	void randomSeed(int seed);

	GameController*		m_gameController;
	GameBoard*			m_gameBoard;
	Timer*				m_Timer;
	GameState*			m_gameState;

	std::vector<DNA*>	m_chromos;

	int					m_chromoIndex;
	int					m_DNAIndex;
	int					m_geneIndex;
	int					m_generation;
	
};

