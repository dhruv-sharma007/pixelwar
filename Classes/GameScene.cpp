#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "Definitions.h"
#include "cocos2d.h"
#include "MainMenuScene.h"
#include "cocostudio/CocoStudio.h"
#include "Character.h"
#include "ui/CocosGUI.h"
#include "LevelManager.h"

USING_NS_CC;

static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

Scene* GameScene::createScene(int levelNumber) {
	auto scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vec2(0, -980));
	auto layer = GameScene::create(levelNumber);
	scene->addChild(layer);

	return scene;
}

GameScene* GameScene::create(int levelNumber) {
	GameScene *pret = new(std::nothrow) GameScene();
	if (pret && pret->init(levelNumber)) {
		pret->autorelease();
		return pret;
	}
	else {
		delete pret;
		pret = nullptr;
		return nullptr;
	}
}

bool GameScene::init(int levelNumber) {
	if (!Layer::init()) {
	}
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	const rapidjson::Value& levelData = LevelManager::getInstance()->getLevelData(levelNumber);

	Director::getInstance()->getTextureCache()->addImage("particle_texture.png");
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("3Objects/Gems/coin.mp3");

	_coinsCount = 0;
	_coinLabel = Label::createWithTTF("Coins: 0", "fonts/Marker Felt.ttf", 36);
	_coinLabel->setPosition(Point(origin.x + visibleSize.width - 100, origin.y + visibleSize.height - 50));
	this->addChild(_coinLabel, 2);

	// Game ui setup --->

	const rapidjson::Value& obstacles = levelData["obstacles"];
	for (int i = 0; i < obstacles.Size(); i++) {
		createObstacle(obstacles[i]);
	}

	// Character setup
	_characterAnimation = Character::create();
	_characterAnimation->initAnimations("1MainCharacters/2");
	_characterAnimation->setPosition(Point(0, 0));
	auto characterSprite = Sprite::create("1MainCharacters/2/Jump.png");

	_character = Node::create();
	_character->setName("character");
	_character->addChild(_characterAnimation);
	this->setName("gameLayer");

	auto characterBody = PhysicsBody::createBox(characterSprite->getContentSize());
	characterBody->setDynamic(true);
	characterBody->setRotationEnable(false);
	characterBody->setMass(1.0f);
	characterBody->setContactTestBitmask(true);
	_character->setPhysicsBody(characterBody);

	const rapidjson::Value& characterPosition = levelData["player_start"];
	
	_character->setPosition(characterPosition["x"].GetFloat(), characterPosition["y"].GetFloat());
	
	this->addChild(_character, 100);

	//character stand
	auto stand = Sprite::create("3Objects/Boxes/1_Idle.png");
	const rapidjson::Value& standPos = levelData["stand_position"];
	stand->setPosition(standPos["x"].GetFloat(), standPos["y"].GetFloat());

	auto standBody = PhysicsBody::createBox(stand->getContentSize());
	standBody->setDynamic(false);
	standBody->setRotationEnable(false);
	standBody->setContactTestBitmask(true);
	stand->setPhysicsBody(standBody);
	stand->setName("stand");
	this->addChild(stand, 101);

	GameUiSetup();

	//Event listner for physicss
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

	auto sampleCoin = Sprite::create("3Objects/Gems/coinImg.png");
	float coinWidth = sampleCoin->getContentSize().width;

	const rapidjson::Value& coins = levelData["coins"];
	_totalCoins = coins.Size();
	for (size_t i = 0; i < _totalCoins; i++)
	{
		float xPosition = coins[i]["x"].GetFloat();
		float yPosition = coins[i]["y"].GetFloat();

		createCoin(Point(xPosition, yPosition));
	}

	// EventListener
	this->scheduleUpdate();
	return true;
}

void GameScene::update(float dt){
	auto body = _character->getPhysicsBody();
	float speed = 200.0f;
	float currentVelocityX = 0;

	if (_isAPressed) {
		currentVelocityX -= speed;
	}
	if (_isDPressed) {
		currentVelocityX += speed;
	}

	body->setVelocity(Vec2(currentVelocityX, body->getVelocity().y));

	if (_isJumping) {
		body->applyImpulse(Vec2(0, 500));
		_isJumping = false;
	}
}

void GameScene::GoToMainMenu(float dt){
	auto scene = MainMenuScene::createScene(0);
	Director::getInstance()->replaceScene(scene);
}

void GameScene::jumpCharacter() {
	if (!_isJumping) {
		auto body = _character->getPhysicsBody();
		body->setVelocity(Vec2(body->getVelocity().x, 0)); 
		body->applyImpulse(Vec2(0, 500));
		_isJumping = true;
	}
}

void GameScene::PauseGame(cocos2d::Ref *) {
	Director::getInstance()->pause();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto pauseLayer = LayerColor::create(Color4B(0, 0, 0, 180));
	pauseLayer->setName("PauseLayer");

	auto label = Label::createWithTTF("Game Paused", "fonts/Marker Felt.ttf", 48);
	label->setPosition(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2 + 100);
	pauseLayer->addChild(label);

	auto resumeButton = MenuItemLabel::create(Label::createWithTTF("Resume", "fonts/Marker Felt.ttf", 36), [&](Ref*) {
		Director::getInstance()->resume();
		this->removeChildByName("PauseLayer");
	});

	auto quitButton = MenuItemLabel::create(
		Label::createWithTTF("Quit to Menu", "fonts/Marker Felt.ttf", 36),
		[&](Ref*) {
		Director::getInstance()->resume();
		auto scene = MainMenuScene::createScene(0);
		Director::getInstance()->replaceScene(scene);
	});

	auto restartBtn = MenuItemLabel::create(Label::createWithTTF("Restart Game", "fonts/Marker Felt.ttf", 36), [&](Ref* sender) {
		Director::getInstance()->resume();
		auto scene = GameScene::createScene(LevelManager::getInstance()->getCurrentLevel());
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));
	});

	auto menu = Menu::create(resumeButton,restartBtn, quitButton, nullptr);
	menu->alignItemsVerticallyWithPadding(20);
	menu->setPosition(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2);
	pauseLayer->addChild(menu);
	this->addChild(pauseLayer, 100);
}

void GameScene::createCoin(Point position) {

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto coin = Sprite::create("3Objects/Gems/coinImg.png");
	coin->setPosition(position);

	auto coinBody = PhysicsBody::createCircle(coin->getContentSize().width / 2);
	coinBody->setDynamic(false); 
	coinBody->setContactTestBitmask(true);
	coin->setPhysicsBody(coinBody);
	coin->setName("coin");  
	this->addChild(coin, 100);
}

bool GameScene::onContactBegin(cocos2d::PhysicsContact& contact) {
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();

	if (nodeA && nodeB) {
		if (nodeA->getName() == "coin" && nodeB->getName() == "character") {
			collectCoin(nodeA);
		}
		else if (nodeA->getName() == "character" && nodeB->getName() == "coin") {
			collectCoin(nodeB);
		} 
		else if ((nodeA->getName() == "character" && nodeB->getName() == "ground") || (nodeA->getName() == "ground" && nodeB->getName() == "character")) {

			auto prizeLabel = Label::createWithTTF("You loose the level Dont touch red area", "fonts/Marker Felt.ttf", 48);
			prizeLabel->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2, 100));
			this->addChild(prizeLabel, 10);
			prizeLabel->runAction(Sequence::create(DelayTime::create(2.0f), FadeOut::create(1.0f), RemoveSelf::create(), nullptr));

			auto playerCoins = UserDefault::getInstance()->getIntegerForKey("coins", 0);
			if (playerCoins > 0) {
				UserDefault::getInstance()->setIntegerForKey("coins", playerCoins - _coinsCount);
			}
			auto scene = GameScene::createScene(LevelManager::getInstance()->getCurrentLevel());
			Director::getInstance()->replaceScene(TransitionFade::create(2.5f, scene));
		}
		else if ((nodeA->getName() == "character" && nodeB->getName() == "stand") || (nodeA->getName() == "stand" && nodeB->getName() == "character")) {
			if (_coinsCount == _totalCoins) {
				
				this->getEventDispatcher()->removeEventListenersForTarget(this);
				_character->getPhysicsBody()->setVelocity(Vec2::ZERO); 
				LevelManager::getInstance()->increaseLevel();
				goToMainMenuScene();
			}
		}
	}

	return true;
}
	
void GameScene::collectCoin(Node* coin) {
	// Bug: Delay win showing particle effect
	auto coinPosition = coin->getPosition();
	coin->removeFromParent();

	auto coinEffect = ParticleSystemQuad::create("particle/particle_texture.plist");
	coinEffect->setDuration(0.1);
	coinEffect->setPosition(coinPosition);
	coinEffect->setAutoRemoveOnFinish(true);
	coinEffect->setScale(0.38f);
	this->addChild(coinEffect, 100);
	_coinsCount++;
	_coinLabel->setString("Coins: " + std::to_string(_coinsCount));
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("3Objects/Gems/coin.mp3");
}

void GameScene::GameUiSetup() {

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();
	
	auto pauseItem = MenuItemImage::create("5GUI/pause.png", "5GUI/pause.png", CC_CALLBACK_1(GameScene::PauseGame, this));
	pauseItem->setPosition(Point((visibleSize.width* 0.05f) + origin.x, (visibleSize.height* 0.9f) + origin.y));
	pauseItem->setScale(0.1f);

	auto menu = Menu::create(pauseItem, nullptr);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);

	auto edgeBody = PhysicsBody::createEdgeBox(visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 3);
	edgeBody->setContactTestBitmask(true);

	auto edgeNode = Node::create();
	edgeNode->setPosition(Point(Point(visibleSize.width / 2, visibleSize.height / 2 + origin.y)));
	edgeNode->setPhysicsBody(edgeBody);

	this->addChild(edgeNode);

	auto bg = Sprite::create("2Locations/Backgrounds/4.png");
	if (!bg) {
		problemLoading("2Locations/Backgrounds/4.png");
	}
	else {
		bg->setPosition(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height / 2);
		bg->setScaleX(visibleSize.width / bg->getContentSize().width);
		bg->setScaleY(visibleSize.height / bg->getContentSize().height);
		this->addChild(bg);
	}
	auto leftButton = ui::Button::create("5GUI/Buttons/Button_01.png", "5GUI/Buttons/Button_02.png");
	leftButton->setScale(2.0f);
	leftButton->setPosition(Point(origin.x + leftButton->getContentSize().width / 2 + 50,
		origin.y + leftButton->getContentSize().height / 2 + 50));
	leftButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			_isAPressed = true;
			break;
		case ui::Widget::TouchEventType::ENDED:
		case ui::Widget::TouchEventType::CANCELED:
			_isAPressed = false;
			break;
		default:
			break;
		}
	});
	this->addChild(leftButton, 200);

	auto rightButton = ui::Button::create("5GUI/Buttons/Button_03.png", "5GUI/Buttons/Button_04.png");
	rightButton->setScale(2.0f);
	rightButton->setPosition(Vec2(leftButton->getPosition().x + leftButton->getContentSize().width + 50,
		leftButton->getPosition().y));
	rightButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			_isDPressed = true;
			_characterAnimation->run(true);
			break;
		case ui::Widget::TouchEventType::ENDED:
		case ui::Widget::TouchEventType::CANCELED:
			_isDPressed = false;
			_characterAnimation->idle();
			break;
		default:
			break;
		}
	});
	this->addChild(rightButton, 201);

	auto jumpButton = ui::Button::create("5GUI/Buttons/Button_05.png", "5GUI/Buttons/Button_06.png");
	jumpButton->setScale(2.0f);
	jumpButton->setPosition(Vec2(origin.x + visibleSize.width - 100,
		origin.y + jumpButton->getContentSize().height / 2 + 50));
	jumpButton->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type) {
		switch (type)
		{
		case ui::Widget::TouchEventType::BEGAN:
			_isJumping = true;
			_characterAnimation->jump();
			break;
		case ui::Widget::TouchEventType::ENDED:
		case ui::Widget::TouchEventType::CANCELED:
			_characterAnimation->idle();
			_isJumping = false;
			break;
		default:
			break;
		}
	});
	this->addChild(jumpButton, 203);

	// Keyboard event

	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_A:
			_isAPressed = true;
			_characterAnimation->run(false);
			break;
		case EventKeyboard::KeyCode::KEY_D:
			_characterAnimation->run(true);
			_isDPressed = true;
		case EventKeyboard::KeyCode::KEY_SPACE:
			_characterAnimation->jump();
			_isJumping = true;
		}
	};

	listener->onKeyReleased = [&](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_A:
			_isAPressed = false;
			_characterAnimation->idle();
			break;
		case EventKeyboard::KeyCode::KEY_D:
			_isDPressed = false;
			_characterAnimation->idle();
			break;
		default:
			break;
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameScene::createObstacle(const rapidjson::Value& obstacleData) {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	std::string type = obstacleData["type"].GetString();
	Sprite* obstacle;

	// Todo: Implement is ground red area
	if (type == "ground") {
		obstacle = Sprite::create("2Locations/Tiles/Tile_85.png");
		obstacle->setName("ground");
	}
	else if (type == "floating_platform") {
		obstacle = Sprite::create("2Locations/Tiles/Tile_84.png");
		obstacle->setName("floating_platform");
	}

	auto width = obstacleData["width"].GetFloat();
	auto height = obstacleData["height"].GetFloat();
	Point position = Point(obstacleData["x"].GetFloat() + origin.x, obstacleData["y"].GetFloat() + origin.y);
	// obstacle->setTextureRect(Rect(0,0,width * 2, height));
	obstacle->setPosition(position);

	// Todo: implement is obstacle isCollidable
	// Todo: 
	auto obstacleBody = PhysicsBody::createBox(obstacle->getContentSize());
	obstacleBody->setDynamic(false);
	obstacleBody->setContactTestBitmask(true);

	obstacle->setPhysicsBody(obstacleBody);

	this->addChild(obstacle, 1);
}

void GameScene::LevelWonAnimation(int coins) {
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
			ScaleTo::create(0.1f, 1.0f),
			MoveTo::create(flyDuration, endPosition),
			CallFunc::create([this, &currentLabelCount]() {}),
			nullptr
		);
		coin->runAction(action);
	}

	float totalAnimationTime = (coins + delayBetweenCoins) + flyDuration;
	auto finalTransitionAction = Sequence::create(
		DelayTime::create(totalAnimationTime + 0.5f), // Add a small buffer
		CallFunc::create(CC_CALLBACK_0(GameScene::goToMainMenuScene, this)),
		nullptr
	);

	this->runAction(finalTransitionAction);
}

void GameScene::goToMainMenuScene() {
	auto scene = MainMenuScene::createScene(_totalCoins);
	Director::getInstance()->replaceScene(scene);
}