#include "cocos2d.h"
#include "WheelScene.h"
#include "Definitions.h"
#include "MainMenuScene.h"
USING_NS_CC;

#define WHEEL_SPINS 5

Scene* WheelScene::createScene() {
	auto scene = Scene::create();
	auto wheelLayer = WheelScene::create();
	scene->addChild(wheelLayer);
	return scene;
}

bool WheelScene::init() {
	if (!Layer::init()) {
		return false;
	}

	_isSpinning = false;
	_prizes = { 10, 0, 20, 100, 50, 30, 10, 0, 20, 100, 50, 30 };

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Vec2 center = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

	auto bg = Sprite::create("5GUI/BG.png");

	if (bg == nullptr) {
		log("Error: Failed to load '5GUI/BG.png'.");
		return false;
	}
	bg->setPosition(center);
	this->addChild(bg);

	_wheel = Sprite::create("5GUI/spin-wheel.png");

	if (!_wheel) {
		log("Error: Failed to load '5GUI/spin-wheel.png'.");
		return false;
	}
	_wheel->setPosition(center);
	this->addChild(_wheel, 2);

	auto arrow = Sprite::create("5GUI/arrow.png");
	arrow->setPosition(Vec2(center.x, center.y + _wheel->getContentSize().height / 2 + 15));
	this->addChild(arrow, 3);

	_spinButton = MenuItemImage::create("5GUI/spin.png", "5GUI/spin.png", CC_CALLBACK_1(WheelScene::onSpinClick, this));
	_spinButton->setPosition(Point(origin.x + visibleSize.width / 2, (origin.y + visibleSize.height / 2) - (_wheel->getContentSize().height / 1.2)));

	auto crossItem = MenuItemImage::create("5GUI/cross.png", "5GUI/cross.png", [&](cocos2d::Ref* sender) {
		auto scene = MainMenuScene::createScene(0);
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));
	});
	crossItem->setPosition(Point(visibleSize.width + origin.x - 50,
		visibleSize.height + origin.y - 50));

	auto menu = Menu::create(_spinButton, crossItem, nullptr);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	return true;
}

void WheelScene::onSpinClick(cocos2d::Ref* sender) {

	if (_isSpinning) {
		return;
	}
	_isSpinning = true;
	_spinButton->setEnabled(false);

	int segmentCount = _prizes.size();
	float degreesPerSegment = 360.0f / segmentCount;
	auto randomNum = cocos2d::RandomHelper::random_int(0, segmentCount - 1);

	float targetAngle = (randomNum * degreesPerSegment) + (degreesPerSegment / 2);	
	targetAngle -= 15;
	auto finalRotation = -((360 * WHEEL_SPINS) + targetAngle);
	auto rotateAction = RotateBy::create(5.0f, finalRotation);
	auto easedAction = EaseBackOut::create(rotateAction);

	auto onComplete = CallFunc::create([this, randomNum]() {
		this->spinCompleted(randomNum);
	});

	auto sequence = Sequence::create(easedAction, onComplete, nullptr);
	_wheel->runAction(sequence);
}

void WheelScene::spinCompleted(int idx) {
	auto coins = _prizes[idx];

	if (coins > 0) {
		int currentCoins = UserDefault::getInstance()->getIntegerForKey("coins", 0);
		UserDefault::getInstance()->setIntegerForKey("coins", coins + currentCoins);
		UserDefault::getInstance()->flush();
	}

	auto prizeLabel = Label::createWithTTF("You won " + std::to_string(coins) + " coins!", "fonts/Marker Felt.ttf", 48);
	prizeLabel->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2, 100));
	this->addChild(prizeLabel, 10);
	prizeLabel->runAction(Sequence::create(DelayTime::create(2.0f), FadeOut::create(1.0f), RemoveSelf::create(), nullptr));

	_isSpinning = false;
	_spinButton->setEnabled(true);
}
