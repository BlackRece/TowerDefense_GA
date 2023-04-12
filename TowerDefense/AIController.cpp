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
	
	m_GAManager = new GAManager();
}

AIController::~AIController()
{

}

void AIController::gameOver()
{
	int score = recordScore();
	int kills = m_gameState->getMonsterEliminated();
	int duration = floor(m_Timer->elapsedSeconds());
	
	m_GAManager->storeFitnessData(score, kills, duration);
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
	
	// attempt place tower
	FailReason wasPlaced = addTower(m_GAManager->getGene());
		
	switch (wasPlaced)
	{
		case DidNotFail: m_GAManager->markGene();
		case Position: m_GAManager->nextGene();
		case Cost: /* Do nothing and try again once funds available */ break;
	}
	
	//GAManager::Instance()->Update(m_Timer->elapsedSeconds());
	
	recordScore();
}

AIController::FailReason AIController::addTower(Gene& gene)
{
	// NOTE: addTower returns false if;
	// - the tower can't be placed in that position, 
	// - is there isn't enough funds

	// check if funds are available
	if (m_gameState->getTams() < m_gameState->getTowerProps(gene.m_towerType)["tam"])
		return Cost;

	// attempt to add
	return m_gameBoard->addTower(gene.m_towerType, gene.m_towerPosition.x, gene.m_towerPosition.y)
		? DidNotFail
		: Position;
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

