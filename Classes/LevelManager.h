#pragma once

#include "cocos2d.h"
#include "json/rapidjson.h"
#include "json/document.h"

class LevelManager {
public:
	static LevelManager* getInstance();
	const rapidjson::Value& getLevelData(int levelNumber);

	int getCurrentLevel();
	bool increaseLevel();
	bool decreaseLevel();

private:
	LevelManager();
	rapidjson::Document _document;
};