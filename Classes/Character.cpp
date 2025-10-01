#include "Character.h"
#include "Definitions.h"

USING_NS_CC;

bool Character::init() {

	if (!Sprite::init()) {
		return false; 
	}

	_health = 100;
	_speed = 150.0f;
	_attackPower = 10;

	this->scheduleUpdate();

	return true;
}

void Character::initAnimations(const std::string& directory) {

	saveAnimation(directory + "/Idle.png", IDLE_FRAMES, FRAME_SPEED, CharacterState::IDLE);
	saveAnimation(directory + "/Run.png", RUN_FRAMES, FRAME_SPEED, CharacterState::RUNNING);
	saveAnimation(directory + "/Jump.png", JUMP_FRAMES, FRAME_SPEED, CharacterState::JUMPING);
	saveAnimation(directory + "/Fall.png", FALL_FRAMES, FRAME_SPEED, CharacterState::FALL);
	saveAnimation(directory + "/Attack.png", HIT_FRAMES, FRAME_SPEED, CharacterState::ATTACKING);
	
	_currentState = CharacterState::IDLE;
	this->playAnimation(_currentState);
}

void Character::saveAnimation(const std::string& filename, int frames, float frameSpeed, CharacterState characterState) {
	auto texture = Director::getInstance()->getTextureCache()->addImage(filename);
	if (!texture) return;

	float frameWidth = texture->getContentSize().width / frames;
	float frameHeight = texture->getContentSize().height;

	Vector<SpriteFrame*> spriteFrames;
	for (int i = 0; i < frames; ++i) {
		auto rect = Rect(i * frameWidth, 0, frameWidth, frameHeight);
		spriteFrames.pushBack(SpriteFrame::createWithTexture(texture, rect));
	}

	auto animation = Animation::createWithSpriteFrames(spriteFrames, frameSpeed);
	auto animate = Animate::create(animation);

	animate->retain();

	_animations.insert(characterState, animate);
}

void Character::playAnimation(CharacterState characterState) {

	if (_animations.find(characterState) == _animations.end()) {
		return;
	}

	this->stopAllActions();
	auto animationToPlay = _animations.at(characterState);
	this->runAction(RepeatForever::create(animationToPlay));
}

void Character::update(float dt) {
	
}

void Character::idle() {
	if (_currentState == CharacterState::IDLE) return;

	_currentState = CharacterState::IDLE;
	playAnimation(_currentState);
}

void Character::run(bool moveRight) {
	if (_currentState == CharacterState::RUNNING) return;
	_currentState = CharacterState::RUNNING;
	playAnimation(_currentState);
}

void Character::jump() {
	if (_currentState == CharacterState::JUMPING) return;
	_currentState = CharacterState::JUMPING;

	auto jumpSequence = Sequence::create(
		_animations.at(CharacterState::JUMPING),
		CallFunc::create([this]() {
		this->_currentState = CharacterState::FALL;
		this->playAnimation(CharacterState::FALL);
	}),
		nullptr
		);
	this->stopAllActions();
	this->runAction(jumpSequence);
}

void Character::attack() {
	if (_currentState == CharacterState::ATTACKING || _currentState == CharacterState::DEAD) return;

	_currentState = CharacterState::ATTACKING;

	auto attackSequence = Sequence::create(
		_animations.at(CharacterState::ATTACKING),
		CallFunc::create([this]() { this->idle(); }),
		nullptr
	);
	this->stopAllActions();
	this->runAction(attackSequence);
}

void Character::takeDamage(int amount) {
	
}