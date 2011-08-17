#ifndef _MONSTER_H_
#define _MONSTER_H_

#include "cocos2d.h"

class Monster : public cocos2d::CCSprite 
{
	
private:
	
public:
	CC_SYNTHESIZE(int, _curHp, CurHp);
	CC_SYNTHESIZE(int, _minMoveDuration, MinMoveDuration);
	CC_SYNTHESIZE(int, _maxMoveDuration, MaxMoveDuration);
};

class WeakAndFastMonster : public Monster
{
public:
	static WeakAndFastMonster* monster() {
		
		WeakAndFastMonster *monster = NULL;
		
		monster = new WeakAndFastMonster();
		if (monster && monster->initWithFile("Target2.jpg"))
		{
			monster->setCurHp(1);
			monster->setMinMoveDuration(3);
			monster->setMaxMoveDuration(5);
		}
		else {
			delete monster;
		}
		
		return monster;
	};	
};

class StrongAndSlowMonster : public Monster
{
public:
	static StrongAndSlowMonster* monster() {
		
		StrongAndSlowMonster *monster = NULL;
		
		monster = new StrongAndSlowMonster();
		if (monster && monster->initWithFile("Target2.jpg"))
		{
			monster->setCurHp(3);
			monster->setMinMoveDuration(6);
			monster->setMaxMoveDuration(12);
		}
		else {
			delete monster;
		}
		
		return monster;
	};	
};

#endif