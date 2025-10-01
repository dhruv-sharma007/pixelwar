#pragma once
#include "cocos2d.h"
#include "json/rapidjson.h"
#include "json/document.h"	
#include "Character.h"

class GameScene : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene(int levelNumber);
	virtual bool init(int levelNumber);

	private: 
		cocos2d::Node* _character;
		bool _isAPressed;
		bool _isDPressed;
		bool _isJumping;
		int _coinsCount;
		int _totalCoins;
		Character* _characterAnimation;
		cocos2d::Label* _coinLabel;

		static GameScene* create(int levelNumber);
		bool onContactBegin(cocos2d::PhysicsContact& contact);
		void collectCoin(cocos2d::Node* coin);
		void createCoin(cocos2d::Point position);
		void jumpCharacter();
		void update(float dt) override;
		void PauseGame(cocos2d::Ref* sender);
		void GoToMainMenu(float dt);
		void Exit(float dt);
		void GameUiSetup();
		void createObstacle(const rapidjson::Value& obstacleData);
		void goToMainMenuScene();

		void LevelWonAnimation(int coins);
	};