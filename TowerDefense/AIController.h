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
		m_spawnDelay = (int)(rand() % SPAWN_DELAY);
		m_score = 0;
		m_kills = 0;
	}
	~Gene() {}
	
	TowerType m_towerType;
	sf::Vector2f m_towerPosition;
	int m_spawnDelay;
	int m_score;
	int m_kills;
};

struct DNA
{
	std::vector<Gene*> m_genes;
	int m_score;
	int m_kills;
	int m_duration;
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

	GameController*		m_gameController;
	GameBoard*			m_gameBoard;
	Timer*				m_Timer;
	GameState*			m_gameState;

	std::vector<DNA>	m_chromos;

	int					m_chromoIndex;
	int					m_DNAIndex;
	int					m_geneIndex;
	
};

