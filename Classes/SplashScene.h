#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class SplashScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();
	
	virtual bool init();
	
private:
	CREATE_FUNC(SplashScene);
	void GoToMainMenu(float dt);
	cocos2d::ui::LoadingBar* _loadingBar;
};

//