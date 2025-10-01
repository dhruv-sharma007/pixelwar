#pragma once
#include "cocos2d.h"

class MainMenuScene: public cocos2d::Scene{
public:
	bool virtual init(int coins);
	static Scene* createScene(int coins);

private:
	cocos2d::Label* _coinLabel;
	int _coinsWinned;

	static MainMenuScene* create(int coins);
	void GoToGameScene(cocos2d::Ref* sender);
	void GoToWheelScene(cocos2d::Ref* sender);
	void ExitGame();
	void showNameInputPopup();
	void showCoinAnimation(int coins);
};