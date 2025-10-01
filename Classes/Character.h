#pragma once

#include "cocos2d.h"

enum class CharacterState
{
	IDLE,
	RUNNING,
	JUMPING,
	ATTACKING,
	HURT,
	DEAD,
	FALL
};

namespace std {
	template <>
	struct hash<CharacterState> {
		size_t operator()(const CharacterState& state) const {
			return static_cast<size_t>(state);
		}
	};
}

class Character : public cocos2d::Sprite {
public:

	virtual bool init() override;
	void update(float dt) override;

	void attack();
	void jump();
	void run(bool moveRight);
	void idle();
	void takeDamage(int amount);
	void initAnimations(const std::string& directory);

	CREATE_FUNC(Character);
private:
	int _health;
	int _attackPower;
	float _speed;

	bool _isMovingRight;
	CharacterState _currentState;
	
	cocos2d::Map<CharacterState, cocos2d::Animate*> _animations;

	void saveAnimation(const std::string& filename, int frames, float frameSpeed, CharacterState characterState);
	void playAnimation(CharacterState state);
};