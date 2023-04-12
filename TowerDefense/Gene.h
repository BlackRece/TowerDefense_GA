#pragma once
#ifndef Gene_H
#define Gene_H

#include "Tower.h"
#include <SFML/Graphics.hpp>
#include <map>

constexpr int MAX_COL = 25;
constexpr int MAX_ROW = 17;

class Gene
{
public:
	Gene();
	~Gene() {}

	void copy(Gene& gene);
	void mutate();
	
	inline sf::Vector2f getPosition() { return m_towerPosition; }
	inline sf::Vector2f getRandomPosition() { return sf::Vector2f(rand() % MAX_COL, rand() % MAX_ROW); }
	
	inline TowerType getRandomTower() { return (TowerType)(rand() % 3 + 1); }
	
	TowerType m_towerType;
	sf::Vector2f m_towerPosition;

private:
	bool			m_isAnyPlaced;
	bool			m_isPlaced;
};

#endif // !Gene_H