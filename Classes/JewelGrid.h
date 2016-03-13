// JewelGrid.h
#pragma once
#include "cocos2d.h"
#include <vector>

USING_NS_CC;
using namespace std;

const int GRID_WIDTH = 40;
const double MOVE_SPEED = 0.2;
const int FIRST_JEWEL_ID = 1;
const int LAST_JEWEL_ID = 6;

class Jewel;

class JewelGrid : public Node
{
public:
	static JewelGrid* create(int row, int col);
	bool init(int row, int col);

	void updateMap();
	bool isDeadMap();

private:
	Jewel* createAJewel(int x, int y);
	bool isJewelLegal(Jewel * jewel, int x, int y);
	void setJewelPixPos(Jewel * jewel, float x, float y);

	void swapJewels(Jewel * jewelA, Jewel * jewelB);
	void swapJewelToNewPos(Jewel * jewel, float x, float y);

	void refreshJewelGrid();
	void refreshJewelToNewPos(int col);

private:
	bool onTouchBegan(Touch *, Event *);
	void onTouchMoved(Touch *, Event *);

private:
	bool canCrush();
	void doCrush();

	void onJewelsSwapping(float dt);
	void onJewelsSwappingBack(float dt);
	void onJewelsCrushing(float dt);
	void onJewelsRefreshing(float dt);

public:
	int getRow() { return m_row; }
	int getCol() { return m_col; }

private:
	int m_row;
	int m_col;

	Jewel * m_jewelSelected;
	Jewel * m_jewelToSwap;

	vector<vector<Jewel*>> m_JewelBox;
	Vector<Jewel*> m_crushingJewelBox;
	Vector<Jewel*> m_newJewelBox;
};