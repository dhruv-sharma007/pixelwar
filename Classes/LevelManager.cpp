#include "LevelManager.h"
#include "cocos2d.h"
#include "Definitions.h"

USING_NS_CC;

LevelManager* LevelManager::getInstance() {
	static LevelManager instance;
	return &instance;
}

LevelManager::LevelManager() {
	std::string fullPath = FileUtils::getInstance()->fullPathForFilename("levels.json");
	std::string fileContent = FileUtils::getInstance()->getStringFromFile(fullPath);

	_document.Parse(fileContent.c_str());
	if (_document.HasParseError()) {
		log("Failed to parse levels.json");
	}
}

const rapidjson::Value& LevelManager::getLevelData(int levelNumber) {
	const rapidjson::Value& levels = _document["levels"];


	if (levelNumber > 0 && levelNumber <= levels.Size()) {
		return levels[levelNumber - 1];
	}

	log("Error: Tried to load invalid level number: %d. Defaulting to level 1.", levelNumber);
	return levels[0];
}

int LevelManager::getCurrentLevel() {
	int level = UserDefault::getInstance()->getIntegerForKey(CURRENT_LEVEL_USERDEFAULT, 1);
	return level;
}

bool LevelManager::increaseLevel() {
	int currentLevel = this->getCurrentLevel();
	// Add a check to not go past the max level
	int maxLevel = _document["levels"].Size();
	if (currentLevel < maxLevel) {
		UserDefault::getInstance()->setIntegerForKey(CURRENT_LEVEL_USERDEFAULT, currentLevel + 1);
		UserDefault::getInstance()->flush(); // Flush after writing
	}
	return true;
}

bool LevelManager::decreaseLevel() {
	int currentLevel = this->getCurrentLevel();
	if (currentLevel > 1){
		UserDefault::getInstance()->setIntegerForKey(CURRENT_LEVEL_USERDEFAULT, currentLevel - 1);
		UserDefault::getInstance()->flush(); // Flush after writing
		return true;
	}
	return false;
}