#include "SplashScene.h"
#include "SimpleAudioEngine.h"
#include "Definitions.h"
#include "cocos2d.h"
#include "MainMenuScene.h"
#include "Character.h"
#include "Networking.h"

USING_NS_CC;

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

Scene* SplashScene::createScene() {
	return create();
}

bool SplashScene::init() {
	if (!Scene::init()) {
		return false;
	}
	
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("game-music.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("retro.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("3Objects/Gems/coin.mp3");

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	_loadingBar = ui::LoadingBar::create();
	_loadingBar->setDirection(ui::LoadingBar::Direction::LEFT);
	_loadingBar->setPercent(0);
	_loadingBar->setPosition(Point(visibleSize.width / 2 + origin.x, (visibleSize.height - (visibleSize.height / 4 )) + origin.y));
	this->addChild(_loadingBar, 2);

	auto icon = Character::create();
	icon->initAnimations("1MainCharacters/2");
	icon->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	icon->setScale(3, 3);
	this->addChild(icon);

	this->scheduleOnce(schedule_selector(SplashScene::GoToMainMenu), DISPLAY_TIME_SPLASH_SCENE);

	NetworkingService::getInstance()->signIn("Dhruv Sharma");
	return true;
}	
void SplashScene::GoToMainMenu(float dt){

	auto scene = MainMenuScene::createScene(0);
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));
}