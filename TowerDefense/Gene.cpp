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