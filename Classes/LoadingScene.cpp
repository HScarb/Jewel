// LoadingScene.cpp
#include "LoadingScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
USING_NS_CC;
using namespace CocosDenshion;
const int TOTAL_TEXTURE_NUM = 16;

cocos2d::Scene* LoadingScene::createScene()
{
	auto scene = Scene::create();
	auto layer = LoadingScene::create();
	scene->addChild(layer);
	return scene;
}

bool LoadingScene::init()
{
	if (!Layer::init())
		return;

	auto winSize = Director::getInstance()->getWinSize();

	m_texture_num = 0;
	int *ptexture_num = &m_texture_num;

	auto label = Label::createWithSystemFont("Loading...", "Arial", 36);
	label->setPosition(winSize.width / 2, winSize.height / 2);
	addChild(label);

	// call back after finish loading
	auto addTextureCallback = [ptexture_num](Texture2D * texture)
	{
		(*ptexture_num)++;
		log("load a texture async...");
	};

	// load jewel async
	TextureCache::getInstance()->addImageAsync("jewel1.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("jewel2.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("jewel3.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("jewel4.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("jewel5.png", addTextureCallback);
	TextureCache::getInstance()->addImageAsync("jewel6.png", addTextureCallback);

	// load background

	// load other textures

	schedule(schedule_selector(LoadingScene::onTextureLoading));

	// preload audio
	SimpleAudioEngine::getInstance()->preloadEffect("crush.ogg");
	SimpleAudioEngine::getInstance()->preloadEffect("swapback.ogg");

	return true;
}

void LoadingScene::onTextureLoading(float dt)
{
	if(m_texture_num == TOTAL_TEXTURE_NUM)
	{
		unschedule(schedule_selector(LoadingScene::onTextureLoading));
		log("loading done!");

		auto call = CallFunc::create([]()
		{
			auto scene = GameScene::createScene();
			Director::getInstance()->replaceScene(TransitionFade::create(0.5, scene));
		});

		this->runAction(Sequence::create(DelayTime::create(0.51), call, nullptr));
	}
}