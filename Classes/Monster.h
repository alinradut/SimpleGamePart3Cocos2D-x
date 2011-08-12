#ifndef _MONSTER_H_
#define _MONSTER_H_

#import "cocos2d.h"

class Monster : public cocos2d::CCSprite 
{
	
private:
	
public:
	CC_SYNTHESIZE(int, _curHp, CurHp);
	CC_SYNTHESIZE(int, _minMoveDuration, MinMoveDuration);
	CC_SYNTHESIZE(int, _maxMoveDuration, MaxMoveDuration);
};

class WeakAndFastMonster : Monster
{
	static Monster* monster() {
		
		WeakAndFastMonster *monster = Monster::spriteWithFile("Target.jpg");
		monster.hp = 1;
		monster.minMoveDuration = 3;
		monster.minMoveDuration = 5;
		
		return monster;
	};
}

#endif