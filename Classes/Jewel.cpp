// Jewel.cpp
#include "Jewel.h"

Jewel* Jewel::createByType(int type, int x, int y)
{
	auto jewel = new Jewel();

	if (jewel && jewel->init())
	{
		jewel->autorelease();
		return jewel;
	}
	else
	{
		CC_SAFE_DELETE(jewel);
		return nullptr;
	}
}

bool Jewel::initByType(int type, int x, int y)
{
	if (!Sprite::init())
		return false;

	m_type = type;
	m_x = x;
	m_y = y;
	m_isSwapping = false;
	m_isCrushing = false;

	char name[100] = { 0 };
	sprintf(name, "jewel%d.png", m_type);
	this->initWithTexture(TextureCache::getInstance()->getTextureForKey(name));
	this->setAnchorPoint(Vec2(0, 0));

	return true;
}

void Jewel::crush()
{
	// add bonus

	m_isCrushing = true;
	auto action = FadeOut::create(0.2);
	auto call = CallFunc::create([this]()
	{
		this->removeFromParent();
	});
	this->runAction(Sequence::create(action, call, nullptr));
}