#pragma once

#include "Tower.h"
#include <vector>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#include "GAManager.h"

class GameController;
class GameBoard;
class Timer;
class GameState;

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
	void gameOver();

private:
	enum FailReason
	{
		DidNotFail,
		Cost,
		Position
	};

	FailReason addTower(Gene& gene);

	GameController*		m_gameController;
	GameBoard*			m_gameBoard;
	Timer*				m_Timer;
	GameState*			m_gameState;

	GAManager*			m_GAManager;
};

