// LoadingScene.h
#pragma once
#include "cocos2d.h"

class LoadingScene :public cocos2d::Layer
{
public:
	static cocos2d::Scene * createScene();
	CREATE_FUNC(LoadingScene);
	virtual bool init();

private:
	void onTextureLoading(float dt);

	int m_texture_num;
};