#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "vector"
#include "string"

class WheelScene : public cocos2d::Layer {
public:
	static cocos2d::Scene* createScene();
	
	virtual bool init();
	
private:

	std::vector<int> _prizes;
	cocos2d::Sprite* _wheel;
	cocos2d::MenuItemImage* _spingButton;

	bool _isSpinning;
	cocos2d::MenuItemImage* _spinButton;
	void onSpinClick(cocos2d::Ref* sender);
	void spinCompleted(int index);

	CREATE_FUNC(WheelScene);
};