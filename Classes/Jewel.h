#pragma once
#include "cocos2d.h"
USING_NS_CC;

class Jewel : public Sprite
{
public:
	static Jewel* createByType(int type, int x, int y);
	bool initByType(int type, int x, int y);

public:
	int getType() { return m_type; }
	int getX() { return m_x; }
	int getY() { return m_y; }

	void setX(int x) { m_x = x; }
	void setY(int y) { m_y = y; }

	bool isSwapping() { return m_isSwapping; }
	bool isCrushing() { return m_isCrushing; }

	void setSwapState(bool state) { m_isSwapping = state; }

	void crush();

private:
	int m_type;

	int m_x;
	int m_y;

	bool m_isSwapping;		// 是否在交换过程中
	bool m_isCrushing;		// 是否在消去过程中
};