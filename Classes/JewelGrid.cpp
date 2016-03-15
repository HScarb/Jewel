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

	for (int x = 0; x < m_col; x++)
	{
		for (int y = 0; y < m_row; y++)
		{
			// change with left one
			if(x > 0)
			{
				swap(&m_JewelBox[x][y], &m_JewelBox[x - 1][y]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_JewelBox[x][y], &m_JewelBox[x - 1][y]);
			}
			// change with right
			if(x < m_col - 1)
			{
				swap(&m_JewelBox[x][y], &m_JewelBox[x + 1][y]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_JewelBox[x][y], &m_JewelBox[x + 1][y]);
			}
			// change with above
			if(y < m_row)
			{
				swap(&m_JewelBox[x][y], &m_JewelBox[x][y + 1]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_JewelBox[x][y], &m_JewelBox[x][y + 1]);
			}
			// change with below
			if(y>0)
			{
				swap(&m_JewelBox[x][y], &m_JewelBox[x][y - 1]);
				if (canCrush())
					isDeadMap = false;
				swap(&m_JewelBox[x][y], &m_JewelBox[x][y - 1]);
			}
		}
	}
	// canCrush() will save jewels which can crush into a vector, so clear it
	m_crushingJewelBox.clear();

	return isDeadMap;
}

Jewel* JewelGrid::createAJewel(int x, int y)
{
	
	Jewel * jewel = nullptr;

	while(1)
	{
		jewel = Jewel::createByType(random(FIRST_JEWEL_ID, LAST_JEWEL_ID), x, y);
		if (isJewelLegal(jewel, x, y))
			break;
	}
	setJewelPixPos(jewel, x, y);
	addChild(jewel);

	return jewel;
}

// 判断新加入的jewel在x轴y轴方向是否会三消
// 由于是正方向加入的jewe，只要往负方向判断
// xy坐标小于1时不必判断
// 两轴同时合法方合法
bool JewelGrid::isJewelLegal(Jewel* jewel, int x, int y) {}

void JewelGrid::setJewelPixPos(Jewel* jewel, float x, float y)
{
	jewel->setPosition(x * GRID_WIDTH, y * GRID_WIDTH);
}

void JewelGrid::swapJewels(Jewel* jewelA, Jewel* jewelB)
{
	// pause touch listener
	_eventDispatcher->pauseEventListenersForTarget(this);

	auto temp = m_JewelBox[jewelA->getX()][jewelA->getY()];
	m_JewelBox[jewelA->getX()][jewelA->getY()] = m_JewelBox[jewelB->getX()][jewelB->getY()];
	m_JewelBox[jewelB->getX()][jewelB->getY()] = temp;
	
	auto tempX = jewelA->getX();
	jewelA->setX(jewelB->getX());
	jewelB->setX(tempX);

	auto tempY = jewelA->getY();
	jewelA->setY(jewelB->getY());
	jewelB->setY(tempY);
}

// 将jewel移动到它新的位置
void JewelGrid::swapJewelToNewPos(Jewel* jewel, float x, float y)
{
	jewel->setSwapState(true);
	auto move = MoveTo::create(MOVE_SPEED, Vec2(jewel->getX() * GRID_WIDTH, jewel->getY() * GRID_WIDTH));
	auto call = CallFunc::create([jewel]()
	{
		jewel->setSwapState(false);
	});
	jewel->runAction(Sequence::create(move, call, nullptr));
}

void JewelGrid::refreshJewelGrid()
{
	for (int x = 0; x < m_col;x++)
	{
		int empty_count = 0;
		for (int y = 0; y < m_row;y++)
		{
			auto jewel = m_JewelBox[x][y];
			if (!jewel)
				empty_count++;
		}
		if(empty_count)
		{
			// if there are some blanks in this column, refresh these jewels to new position
			refreshJewelToNewPos(x);
		}
	}
}

// refresh jewels in a column
void JewelGrid::refreshJewelToNewPos(int col)
{
	int n = 0;	// blanks
	auto pJewelBox = &m_JewelBox;	// keep a pointer of JewelBox to pass in lambda

	// old jewels drop first
	for (int y = 0; y < m_row; y++)
	{
		auto jewel = m_JewelBox[col][y];
		if(!jewel)
		{
			// if there is no jewel
			n++;
			continue;
		}
		else if(n != 0)
		{
			// drop
			jewel->setY(jewel->getY() - n);
			auto move = MoveBy::create(0.2, Vec2(0, -n * GRID_WIDTH));
			auto call = CallFunc::create([pJewelBox, jewel]()
			{
				(*pJewelBox)[jewel->getX()][jewel->getY()] = jewel;
			});
			jewel->runAction(Sequence::create(move, call, nullptr));
		}
	}

	// new jewels drop
	int i = n;
	int delta = 1;
	for (auto jewel: m_newJewelBox)
	{
		if(jewel->getX() == col)
		{
			jewel->setY(m_row - i);		// drop i rows. i = crushed jewels

			auto move = MoveBy::create(0.2 * delta++, Vec2(0, -i-- *GRID_WIDTH));
			auto call = CallFunc::create([jewel, pJewelBox, this]()
			{
				(*pJewelBox)[jewel->getX()][jewel->getY()] = jewel;
				// erase jewel from newJewelsBox
				m_newJewelBox.eraseObject(jewel);
			});

			jewel->runAction(Sequence::create(DelayTime::create(0.2), move, call, nullptr));
		}
	}
}

bool JewelGrid::onTouchBegan(Touch* pTouch, Event* pEvent)
{
	// convert to node space
	auto pos = pTouch->getLocation();
	auto pos2 = convertToNodeSpace(pTouch->getLocation());
	log("x = %d, y = %d", pos2.x, pos2.y);

	// if touched on a jewel
	if (Rect(0, 0, m_col * GRID_WIDTH, m_row * GRID_WIDTH).containsPoint(pos))
	{
		int x = pos.x / GRID_WIDTH;
		int y = pos.y / GRID_WIDTH;

		m_jewelSelected = m_JewelBox[x][y];
		return true;
	}
	else
		return false;
}

void JewelGrid::onTouchMoved(Touch* pTouch, Event* pEvent)
{
	if (!m_jewelSelected)
		return;

	int startX = m_jewelSelected->getX();
	int startY = m_jewelSelected->getY();

	auto pos = pTouch->getLocation();
	int touchX = pos.x / GRID_WIDTH;
	int touchY = pos.y / GRID_WIDTH;

	if (!Rect(0, 0, m_col * GRID_WIDTH, m_row * GRID_WIDTH).containsPoint(pos) || Vec2(touchX, touchY) == Vec2(startX, startY))
		return;

	// 对角
	if (abs(startX - touchX) + abs(startY - touchY) != 1)
		return;

	// swap
	m_jewelToSwap = m_JewelBox[touchX][touchY];
	swapJewels(m_jewelSelected, m_jewelToSwap);
	// start swap capturer
	schedule(schedule_selector(JewelGrid::onJewelsSwapping));
}

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

void JewelGrid::onJewelsSwapping(float dt)
{
	if (m_jewelSelected->isSwapping() || m_jewelToSwap->isSwapping())
		return;

	// if finished swapping
	unschedule(schedule_selector(JewelGrid::onJewelsSwapping));
	log("Swap over!");
	log("Can it crush?");

	if(canCrush())
	{
		log("Yes, can crush!");
		m_jewelSelected = nullptr;

		// start crushing
		//SimpleAudioEngine::getInstance()->playEffect("crush.ogg");
		doCrush();
		schedule(schedule_selector(JewelGrid::onJewelsCrushing));
	}
	else
	{
		log("No, can't crush!");
		// swap back
		//SimpleAudioEngine::getInstance()->playEffect("swapback.ogg");
		swapJewels(m_jewelSelected, m_jewelToSwap);
		schedule(schedule_selector(JewelGrid::onJewelsSwappingBack));
	}
}

void JewelGrid::onJewelsSwappingBack(float dt)
{
	if (m_jewelSelected->isSwapping() || m_jewelToSwap->isSwapping())
		return;
	
	unschedule(schedule_selector(JewelGrid::onJewelsSwappingBack));
	log("swapped back.");
	m_jewelSelected = nullptr;
	_eventDispatcher->resumeEventListenersForTarget(this);
}

void JewelGrid::onJewelsCrushing(float dt)
{
	for (auto jewel : m_crushingJewelBox)
	{
		if (jewel->isCrushing())
			return;
	}
	// if all finished crushing
	unschedule(schedule_selector(JewelGrid::onJewelsCrushing));
	m_crushingJewelBox.clear();
	log("crush done!");
	log("begin to refresh!");

	refreshJewelGrid();
	schedule(schedule_selector(JewelGrid::onJewelsRefreshing));
}

void JewelGrid::onJewelsRefreshing(float dt)
{
	if (m_newJewelBox.size() != 0)
		return;

	unschedule(schedule_selector(JewelGrid::onJewelsRefreshing));

	log("refresh done!");
	log("now, can it crush?");

	if(canCrush())
	{
		log("can crush.");
		//SimpleAudioEngine::getInstance()->playEffect("crush.ogg");
		doCrush();
		schedule(schedule_selector(JewelGrid::onJewelsCrushing));
	}
	else
	{
		log("no, can't crush.");

		if(isDeadMap())
		{
			log("can't crush any move, update a new map.");

			auto winSize = Director::getInstance()->getWinSize();
			auto label = Label::createWithTTF("Can't crush any move, Crush!", "fonts/Marker Felt.ttf", 24);
			label->setTextColor(Color4B::BLACK);
			label->setPosition(winSize.width / 2, winSize.height / 2);
			label->setOpacity(0);
			this->getParent()->addChild(label);

			// fadein and fade out
			auto fadein = FadeIn::create(0.5);
			auto fadeout = FadeOut::create(0.5);

			auto call = CallFunc::create([this, label]()
			{
				do
				{
					updateMap();
				} while (isDeadMap());
				label->removeFromParent();
				_eventDispatcher->resumeEventListenersForTarget(this);
			});
			label->runAction(Sequence::create(fadein, DelayTime::create(2.0), fadeout, call, nullptr));
		}
		else
		{
			// not dead map
			_eventDispatcher->resumeEventListenersForTarget(this);
		}
	}
}