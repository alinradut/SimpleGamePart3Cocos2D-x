#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "GameOverScene.h"
#include "Monster.h"

USING_NS_CC;

HelloWorld::~HelloWorld()
{
	if (_targets)
	{
		_targets->release();
		_targets = NULL;
	}
	
	if (_projectiles)
	{
		_projectiles->release();
		_projectiles = NULL;
	}
	
	if (_player)
	{
		_player->release();
		_player = NULL;
	}
	// cpp don't need to call super dealloc
	// virtual destructor will do this
}


CCScene* HelloWorld::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::node();
	
	// 'layer' is an autorelease object
	HelloWorld *layer = HelloWorld::node();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !CCColorLayer::initWithColor( ccc4(255,255,255,255) ) )	{
		return false;
	}

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	CCMenuItemImage *pCloseItem = CCMenuItemImage::itemFromNormalImage(
										"CloseNormal.png",
										"CloseSelected.png",
										this,
										menu_selector(HelloWorld::menuCloseCallback) );
	pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::menuWithItems(pCloseItem, NULL);
	pMenu->setPosition( CCPointZero );
	this->addChild(pMenu, 1);

	CCSize size = CCDirector::sharedDirector()->getWinSize();

	//CCSprite* player = CCSprite::spriteWithFile("Player2.jpg", CCRectMake(0, 0, 27, 40));

	// position the sprite on the center of the screen
	//player->setPosition( ccp(player->getContentSize().width/2, size.height/2) );

	// add the sprite as a child to this layer
	//this->addChild(player, 0);
	
    this->schedule( schedule_selector(HelloWorld::gameLogic), 1.0 );
   
	this->setIsTouchEnabled(true);
	
	_targets = new CCMutableArray<CCSprite*>;
	_projectiles = new CCMutableArray<CCSprite*>;
	
	_nextProjectile = NULL;
	_projectilesDestroyed = 0;
	
	_player = CCSprite::spriteWithFile("Player2.jpg");
	_player->retain();
	_player->setPosition(CCPointMake(_player->getContentSize().width/2, size.height/2));
	this->addChild(_player);
	
	this->schedule(schedule_selector(HelloWorld::update));
	CCLOG("Loading music");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("background-music-aac.caf");
	
	return true;
}

void HelloWorld::gameLogic(cocos2d::ccTime dt)
{
    this->addTarget();
}

void HelloWorld::addTarget()
{
	Monster *target = NULL;
	if ((arc4random() % 2) == 0) 
	{
		target = WeakAndFastMonster::monster();
	} else 
	{
		target = StrongAndSlowMonster::monster();
	}
	
    // Determine where to spawn the target along the Y axis
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	int minY = target->getContentSize().height/2;
	int maxY = winSize.height - target->getContentSize().height/2;
	int rangeY = maxY - minY;
	int actualY = (arc4random() % rangeY) + minY;
	
    // Create the target slightly off-screen along the right edge,
    // and along a random position along the Y axis as calculated above
	target->setPosition(ccp(winSize.width + target->getContentSize().width/2, actualY));
	this->addChild(target);
	
	// Determine speed of the target
	int minDuration = target->getMinMoveDuration();
	int maxDuration = target->getMaxMoveDuration();
	int rangeDuration = maxDuration - minDuration;
	int actualDuration = (arc4random() % rangeDuration) + minDuration;

    // Create the actions
	CCFiniteTimeAction *actionMove = CCMoveTo::actionWithDuration(actualDuration, ccp(-target->getContentSize().width/2, actualY));
	CCFiniteTimeAction *actionMoveDone = CCCallFuncN::actionWithTarget(this, callfuncN_selector(HelloWorld::spriteMoveFinished));
    
	target->setTag(1);
	_targets->addObject(target);
	
    target->runAction(CCSequence::actions(actionMove, actionMoveDone));
}

void HelloWorld::spriteMoveFinished(CCNode* sender)
{
    CCSprite *sprite = (CCSprite *)sender;
    this->removeChild(sprite, true);
	if (sprite->getTag() == 1) 
	{ // target
		_targets->removeObject(sprite);
		GameOverScene *gameOverScene = GameOverScene::node();
		gameOverScene->getLayer()->getLabel()->setString("You Lose! :[");
		CCDirector::sharedDirector()->replaceScene(gameOverScene);
		
	} 
	else if (sprite->getTag() == 2) 
	{ // projectile
		_projectiles->removeObject(sprite);
	}
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void HelloWorld::finishShoot()
{
    // Ok to add now - we've finished rotation!
	this->addChild(_nextProjectile);
	_projectiles->addObject(_nextProjectile);
	
	// Release
	_nextProjectile->release();
	_nextProjectile = NULL;
}

void HelloWorld::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	if (_nextProjectile != NULL)
	{
		return;
	}
	
	CCTouch *touch = (CCTouch *)pTouches->anyObject();
	CCPoint location = touch->locationInView(touch->view());
	location = CCDirector::sharedDirector()->convertToGL(location);
	
	// Set up initial location of projectile
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	_nextProjectile = CCSprite::spriteWithFile("Projectile2.jpg", CCRectMake(0, 0, 20, 20));
	_nextProjectile->retain();
	_nextProjectile->setPosition(ccp(20, winSize.height/2));
	
	// Determine offset of location to projectile
	int offX = location.x - _nextProjectile->getPosition().x;
	int offY = location.y - _nextProjectile->getPosition().y;
	
	// Bail out if we are shooting down or backwards
	if (offX <= 0) return;

	// Play a sound!
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("pew-pew-lei.caf");
	
	// Determine where we wish to shoot the projectile to
	int realX = winSize.width + (_nextProjectile->getContentSize().width/2);
	float ratio = (float) offY / (float) offX;
	int realY = (realX * ratio) + _nextProjectile->getPosition().y;
	CCPoint realDest = ccp(realX, realY);
	
	// Determine the length of how far we're shooting
	int offRealX = realX - _nextProjectile->getPosition().x;
	int offRealY = realY - _nextProjectile->getPosition().y;
	float length = sqrtf((offRealX*offRealX)+(offRealY*offRealY));
	float velocity = 480/1; // 480pixels/1sec
	float realMoveDuration = length/velocity;
	
	// Determine angle to face
	float angleRadians = atanf((float)offRealY / (float)offRealX);
	float angleDegrees = CC_RADIANS_TO_DEGREES(angleRadians);
	float cocosAngle = -1 * angleDegrees;
	_player->setRotation(cocosAngle);
	float rotateSpeed = 0.5 / M_PI; // Would take 0.5 seconds to rotate 0.5 radians, or half a circle
    float rotateDuration = fabs(angleRadians * rotateSpeed);    
	
	_player->runAction(CCSequence::actions(CCRotateTo::actionWithDuration(rotateDuration, cocosAngle),
										   CCCallFunc::actionWithTarget(this, callfunc_selector(HelloWorld::finishShoot)),
										   NULL));
	
	// Move projectile to actual endpoint
	_nextProjectile->runAction(CCSequence::actions(CCMoveTo::actionWithDuration(realMoveDuration, realDest), 
												   CCCallFuncN::actionWithTarget(this, callfuncN_selector(HelloWorld::spriteMoveFinished)),
												   NULL));
	// Add to projectiles array
	_nextProjectile->setTag(2);
}

void HelloWorld::update(cocos2d::ccTime dt)
{
	CCMutableArray<cocos2d::CCSprite*> *projectilesToDelete = new CCMutableArray<CCSprite *>;
	
	CCMutableArray<CCSprite*>::CCMutableArrayIterator it, jt;

	for (it = _projectiles->begin(); it != _projectiles->end(); it++)
	{
		CCSprite *projectile = *it;
		CCRect projectileRect = CCRectMake(
										   projectile->getPosition().x - (projectile->getContentSize().width/2), 
										   projectile->getPosition().y - (projectile->getContentSize().height/2), 
										   projectile->getContentSize().width, 
										   projectile->getContentSize().height);
		bool monsterHit = false;
		
		CCMutableArray<cocos2d::CCSprite*> *targetsToDelete = new CCMutableArray<cocos2d::CCSprite*>;
		for (jt = _targets->begin(); jt != _targets->end(); jt++)
		{
			CCSprite *target = *jt;
			CCRect targetRect = CCRectMake(
										   target->getPosition().x - (target->getContentSize().width/2), 
										   target->getPosition().y - (target->getContentSize().height/2), 
										   target->getContentSize().width, 
										   target->getContentSize().height);
			
			if (CCRect::CCRectIntersectsRect(projectileRect, targetRect)) {
				monsterHit = true;
				Monster *monster = (Monster *)target;
				monster->setCurHp(monster->getCurHp()-1);
				CCLOG("Decreasing HP for monster %@ to %d", monster, monster->getCurHp());
				if (monster->getCurHp() <= 0)
				{
					targetsToDelete->addObject(target);
				}
				else {
					CCLOG("derp");
				}

				break;
			}
		}
		
		for (jt = targetsToDelete->begin(); jt != targetsToDelete->end(); jt++) 
		{
			_targets->removeObject(*jt);
			this->removeChild((*jt), true);
			
			_projectilesDestroyed++;
			if (_projectilesDestroyed > 30) {
				GameOverScene *gameOverScene = GameOverScene::node();
				_projectilesDestroyed = 0;
				gameOverScene->getLayer()->getLabel()->setString("You Win!");
				CCDirector::sharedDirector()->replaceScene(gameOverScene);
			}
		}
		
		if (monsterHit) {
			projectilesToDelete->addObject(*it);
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("explosion.caf");
		}
		
		targetsToDelete->release();
	}
	
	for (it = projectilesToDelete->begin(); it != projectilesToDelete->end(); it++)
	{
		_projectiles->removeObject(*it);
		this->removeChild(*it, true);
	}
	projectilesToDelete->release();
}