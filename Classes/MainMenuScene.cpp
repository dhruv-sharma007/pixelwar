#include "MainMenuScene.h"
#include "cocos2d.h"
#include "iostream"
#include "ui/CocosGUI.h"
#include "GameScene.h"
#include "Definitions.h"
#include "SimpleAudioEngine.h"
#include "WheelScene.h"
#include "Character.h"
#include "LevelManager.h"

USING_NS_CC;

Scene* MainMenuScene::createScene(int winnedCoins) {
	return MainMenuScene::create(winnedCoins);
}

MainMenuScene* MainMenuScene::create(int coins) {
	MainMenuScene *pret = new(std::nothrow) MainMenuScene();
	if (pret && pret->init(coins)) {
		pret->autorelease();
		return pret;
	}
	else {
		delete pret;
		pret = nullptr;
		return nullptr;
	}
}

bool MainMenuScene::init(int coins) {
	if(!Scene::init()){
		return false;
	}
	_coinsWinned = coins;
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	//CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("retro.mp3", true);

	// Background
	auto bg = Sprite::create("5GUI/BG.png");
	bg->setPosition(Point(visibleSize.width / 2 + origin.x, (visibleSize.height - visibleSize.height / 2) + origin.y));
	this->addChild(bg);
	//Play Button
	auto PlayItem = MenuItemImage::create("5GUI/Buttons/playButton.png",
		"5GUI/Buttons/playButtonClicked.png",
		CC_CALLBACK_1(MainMenuScene::GoToGameScene, this));
	PlayItem->setAnchorPoint(Point(1.0f, 0.0f));
	PlayItem->setPosition(Point(visibleSize.width + origin.x, origin.y));

	//Go to Wheel Scene
	auto WheelSceneItem = MenuItemImage::create("5GUI/wheel.png", "5GUI/wheel.png",CC_CALLBACK_1(MainMenuScene::GoToWheelScene, this));
	WheelSceneItem->setPosition(Point(((origin.x + visibleSize.width) / 12) - 50, (origin.y + visibleSize.height) / 2));

	auto menu = Menu::create(PlayItem, WheelSceneItem, NULL);
	menu->setPosition(Point::ZERO);

	this->addChild(menu);

	//coins label
	int coinScore = UserDefault::getInstance()->getIntegerForKey("coins", 0);

	std::string labelText = "Coins: " + std::to_string(coinScore);
	_coinLabel = Label::createWithTTF(labelText, "fonts/Marker Felt.ttf", 28);
	_coinLabel->setPosition(Vec2(visibleSize.width / 9, visibleSize.height * 0.8f));
	this->addChild(_coinLabel, 45);

	//Level label 
	int currentLevel = LevelManager::getInstance()->getCurrentLevel();
	std::string levelText = "Level: " + std::to_string(currentLevel);
	auto levelLabel = Label::createWithTTF(levelText, "fonts/Marker Felt.ttf", 28);
	levelLabel->setPosition(Point(visibleSize.width / 9, 350));
	this->addChild(levelLabel, 46);

	// MainMenuScene::showNameInputPopup();

	if (_coinsWinned > 0) {
		showCoinAnimation(_coinsWinned);
	}

	return true;
}

void MainMenuScene::GoToGameScene(cocos2d::Ref* sender){
	auto currentLevel = LevelManager::getInstance()->getCurrentLevel();
	log("current level", currentLevel);

	Scene* scene = GameScene::createScene(currentLevel);
	//CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}

void MainMenuScene::showNameInputPopup() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto popupBg = LayerColor::create(Color4B(0, 0, 0, 50));
	popupBg->setName("NamePopup");
	this->addChild(popupBg);

	// Input Box
	auto nameField = ui::TextField::create("Enter your name", "fonts/Marker Felt.ttf", 32);
	nameField->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 + 50));
	popupBg->addChild(nameField);

	auto saveButton = ui::Button::create("5GUI/Buttons/Button_01.png", "5GUI/Buttons/Button_02.png");
	saveButton->setTitleText("Save");
	saveButton->setTitleFontSize(28);
	saveButton->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 50));

	saveButton->addClickEventListener([=](Ref* sender) {
		auto name = nameField->getString();
		if (!name.empty()) {
			UserDefault::getInstance()->setStringForKey("PlayerName", name);
			UserDefault::getInstance()->flush();
			CCLOG("Player name saved: %s", name.c_str());
			this->removeChildByName("NamePopup");
		}
	});

	popupBg->addChild(saveButton);
}

void MainMenuScene::showCoinAnimation(int coins) {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto startPosition = Point(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	float delayBetweenCoins = 0.15f;
	float flyDuration = 0.6f;
	Point endPosition = _coinLabel->getPosition();
	int currentLabelCount = UserDefault::getInstance()->getIntegerForKey("coins", 0) - coins;

	for (int i = 0; i < coins; i++)
	{
		auto coin = Sprite::create("3Objects/Gems/coinImg.png");
		coin->setPosition(startPosition);
		coin->setScale(0);
		this->addChild(coin, 200);

		auto action = Sequence::create(
			DelayTime::create(i * delayBetweenCoins),
			EaseIn::create(ScaleTo::create(0.1f, 1.0f), 2.5f),
			EaseOut::create(MoveTo::create(flyDuration, endPosition), 2.5f),
			CallFunc::create([this, &currentLabelCount]() {

				UserDefault::getInstance()->setIntegerForKey("coins", 1 + UserDefault::getInstance()->getIntegerForKey("coins", 0));
				auto userDefaultCoins = UserDefault::getInstance()->getIntegerForKey("coins");

				_coinLabel->setString("Coins: " + std::to_string(userDefaultCoins));
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("3Objects/Gems/coin.mp3");
			}),
			CCRemoveSelf::create(),
			nullptr
		);
		coin->runAction(action);
	}

	float totalAnimationTime = (coins + delayBetweenCoins) + flyDuration;
	auto finalTransitionAction = Sequence::create(
		DelayTime::create(totalAnimationTime + 0.5f),
		nullptr
	);
	this->runAction(finalTransitionAction);
}

void MainMenuScene::GoToWheelScene(cocos2d::Ref* sender) {
	auto scene = WheelScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));
}