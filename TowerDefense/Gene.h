#pragma once
#ifndef Gene_H
#define Gene_H

#include "Tower.h"
#include <SFML/Graphics.hpp>
#include <map>
#include <nlohmann/json.hpp>

constexpr int MAX_COL = 25;
constexpr int MAX_ROW = 17;

struct GeneJson
{
	int m_towerType;
	float m_x, m_y;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(GeneJson, m_towerType, m_x, m_y);
};

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
	
	GeneJson getJson();
	void setJson(GeneJson& json);

	TowerType		m_towerType;
	sf::Vector2f	m_towerPosition;

private:
	bool			m_isPlaced;
};

#endif // !Gene_H