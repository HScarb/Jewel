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
			// if seqCount >= 3 than crush
			if (seqCount >= 3)
			{
				for (int n = 0; n < seqCount;n++)
				{
					auto jewel = m_JewelBox[x][y + n];
					m_crushingJewelBox.pushBack(jewel);
				}
			}
			y += seqCount;
		}
	}
	// 遍历每一行
	for (int y = 0; y < m_row; y++)
	{
		for (int x = 0; x < m_col - 1;)
		{
			seqCount = 1;
			JewelBegin = m_JewelBox[x][y];
			JewelNext = m_JewelBox[x + 1][y];

			while (JewelBegin->getType() == JewelNext->getType())
			{
				seqCount++;
				int nextIndex = x + seqCount;
				if (nextIndex > m_col - 2)
					break;
				JewelNext = m_JewelBox[nextIndex][y];
			}
			if(seqCount >= 3)
			{
				for (int n = 0; n < seqCount;n++)
				{
					auto jewel = m_JewelBox[x + n][y];
					// 有可能存在宝石同行列可消除，那么不能重复储存到消除宝石的盒子
					if (m_crushingJewelBox.find(jewel) != m_crushingJewelBox.end())
						continue;
					m_crushingJewelBox.pushBack(jewel);
				}
			}
			x += seqCount;
		}
	}
	// if crushing jewel box isn't empty, return true
	if (!m_crushingJewelBox.empty())
		return true;
	else
		return false;
}

void JewelGrid::doCrush()
{
	for (auto jewel: m_crushingJewelBox)
	{
		// create new jewel, locate at a line above JewelBox
		auto newJewel = Jewel::createByType(random(FIRST_JEWEL_ID, LAST_JEWEL_ID), jewel->getX(), m_row);
		setJewelPixPos(newJewel, newJewel->getX(), m_row);
		addChild(newJewel);

		m_newJewelBox.pushBack(newJewel);
		m_JewelBox[jewel->getX()][jewel->getY()] = nullptr;
		jewel->crush();
	}
}

void JewelGrid::onJewelsSwapping(float dt) {}

void JewelGrid::onJewelsSwappingBack(float dt) {}

void JewelGrid::onJewelsCrushing(float dt) {}

void JewelGrid::onJewelsRefreshing(float dt) {}