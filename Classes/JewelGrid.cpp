// JewelGrid.cpp
#include "JewelGrid.h"
#include "Jewel.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

JewelGrid* JewelGrid::create(int row, int col)
{
	auto jewelGrid = new JewelGrid();
	if (jewelGrid && jewelGrid->init(row, col))
	{
		jewelGrid->autorelease();
		return jewelGrid;
	}
	else
	{
		CC_SAFE_DELETE(jewelGrid);
		return nullptr;
	}
}

bool JewelGrid::init(int row, int col)
{
	if (!Node::init())
		return false;

	m_row = row;
	m_col = col;

	m_jewelSelected = nullptr;
	m_jewelToSwap = nullptr;

	m_JewelBox.resize(m_row);
	for (auto &vec : m_JewelBox)
		vec.resize(m_col);

	for (int x = 0; x < m_col;x++)
	{
		for (int y = 0; y < m_row;y++)
		{
			m_JewelBox[x][y] = createAJewel(x, y);
		}
	}

	while(isDeadMap())
	{
		log("dead map! need to update.");
		updateMap();
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(JewelGrid::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(JewelGrid::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	log("JewelsGrid init!");
	return true;
}

void JewelGrid::updateMap()
{
	for (int x = 0; x < m_col;x++)
	{
		for (int y = 0; y < m_row;y++)
		{
			m_JewelBox[x][y]->removeFromParent();
			m_JewelBox[x][y] = createAJewel(x, y);
		}
	}
	log("updated a new map");
}

bool JewelGrid::isDeadMap()
{
	auto swap = [](Jewel **a, Jewel **b)
	{
		auto temp = *a;
		*a = *b;
		*b = temp;
	};

	bool isDeadMap = true;

	for (int x = 0; x < m_col;x++)
	{
		for (int y = 0; y < m_row;y++)
		{
			if(x > 0)
			{
				swap(&m_JewelBox[x][y], &m_JewelBox[x - 1][y]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_JewelBox[x][y], &m_JewelBox[x - 1][y]);
			}

			if(x < m_col - 1)
			{
				swap(&m_JewelBox[x][y], &m_JewelBox[x + 1][y]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_JewelBox[x][y], &m_JewelBox[x + 1][y]);
			}
		}
	}
}

Jewel* JewelGrid::createAJewel(int x, int y) {}

bool JewelGrid::isJewelLegal(Jewel* jewel, int x, int y) {}

void JewelGrid::setJewelPixPos(Jewel* jewel, float x, float y) {}

void JewelGrid::swapJewels(Jewel* jewelA, Jewel* jewelB) {}

void JewelGrid::swapJewelToNewPos(Jewel* jewel, float x, float y) {}

void JewelGrid::refreshJewelGrid() {}

void JewelGrid::refreshJewelToNewPos(int col) {}

bool JewelGrid::onTouchBegan(Touch*, Event*) {}

void JewelGrid::onTouchMoved(Touch*, Event*) {}

bool JewelGrid::canCrush()
{
	int seqCount = 0;
	Jewel * JewelBegin = nullptr;
	Jewel * JewelNext = nullptr;

	// 遍历每一列
	for (int x = 0; x < m_col;x++)
	{
		for (int y = 0; y < m_row - 1;)
		{
			seqCount = 1;
			JewelBegin = m_JewelBox[x][y];
			JewelNext = m_JewelBox[x][y + 1];

			while (JewelBegin->getType() == JewelNext->getType())
			{
				seqCount++;
				int nextIndex = y + seqCount;
				if (nextIndex > m_row - 1)
					break;
				JewelNext = m_JewelBox[x][nextIndex];
			}

		}
	}
}

void JewelGrid::doCrush() {}

void JewelGrid::onJewelsSwapping(float dt) {}

void JewelGrid::onJewelsSwappingBack(float dt) {}

void JewelGrid::onJewelsCrushing(float dt) {}

void JewelGrid::onJewelsRefreshing(float dt) {}