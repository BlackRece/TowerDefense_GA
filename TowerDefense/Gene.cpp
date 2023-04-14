#include "Gene.h"

Gene::Gene()
{
	m_towerType = getRandomTower();
	m_towerPosition = getRandomPosition();
	m_isPlaced = false;
}

void Gene::copy(Gene& gene)
{
	m_towerType = gene.m_towerType;
	m_towerPosition = gene.m_towerPosition;
	m_isPlaced = gene.m_isPlaced;
}

void Gene::mutate()
{
	m_towerType = getRandomTower();
	m_towerPosition = getRandomPosition();
}

GeneJson Gene::getJson()
{
	GeneJson json
	{
		json.m_towerType = (int)m_towerType,
		json.m_x = m_towerPosition.x,
		json.m_y = m_towerPosition.y,
	};
	return json;
}

void Gene::setJson(GeneJson& json)
{
	m_towerType = (TowerType)json.m_towerType;
	m_towerPosition = sf::Vector2f(json.m_x, json.m_y);
}
