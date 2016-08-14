#include "MainScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MainScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    float scaleFactor = visibleSize.height / visibleSize.width;

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(MainScene::menuCloseCallback, this));

    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));


    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Main", "fonts/Roboto-Regular.ttf", 24);
    label->setColor(Color3B::BLACK);

    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "MainScene" splash screen"
    auto spriteBackground = Sprite::create("MainBackground.png");
    
    // position the sprite on the center of the screen
    spriteBackground->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(spriteBackground, 0);

	// Net Manager
	std::unique_ptr<NetManager> netManager(new NetManager());
	_NetManager = std::move(netManager);
	_NetManager->Init("gameserver.bitbosh.com", "5000");

	// Initialize buffers to null
	_cSendBuffer[0] = '\0';
	_cRecvBuffer[0] = '\0';

	this->scheduleUpdate();

	// Register for touch events
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	

	_iPlayerOneX = visibleSize.width / 2;
	_iPlayerOneY = visibleSize.height / 2;
	_iPlayerTwoX = 0;
	_iPlayerTwoY = 0;
	
	_PlayerOne = cocos2d::Sprite::create("playerone.png");
	_PlayerOne->setPosition(cocos2d::Vec2(visibleSize.width / 2, _PlayerOne->getContentSize().height));
	this->addChild(_PlayerOne);

	// FireballA
	_FireballA = Fireball::create("objects/fireballA.png");
	_iFireballAX = _FireballA->getContentSize().width / 2;
	_iFireballAY = 0;
	_FireballA->setPosition(cocos2d::Vec2(_iFireballAX, _iFireballAY));
	this->addChild(_FireballA);

	// FireballB
	_FireballB = Fireball::create("objects/fireballB.png");
	_iFireballBX = visibleSize.width - _FireballB->getContentSize().width / 2;
	_iFireballBY = 0;
	_FireballB->setPosition(cocos2d::Vec2(_iFireballBX, _iFireballBY));
	this->addChild(_FireballB);

    return true;
}

void MainScene::update(float dt)
{
	/*
	// Move FireballA forward on touch
	CCLOG("Updating fireballAs position");
	_iFireballAY++;
	_FireballA->setPosition(cocos2d::Vec2(_iFireballAX, _iFireballAY));

	// Send a message to the server about fireballA
	CCLOG("Sending message to the server");
	std::string message = "{uid:001,value:w}";

	sprintf(_cSendBuffer, "%s", message.c_str());
	_NetManager->SendData(_cSendBuffer);
	*/

	_NetManager->ReadData(_cRecvBuffer, MAX_BUFFER_SIZE_RECV);
	if (strcmp(_cRecvBuffer, "0") != 0)
	{		
		// Update fireballBs position according to the server
		if (strcmp(_cRecvBuffer, "{uid:002,value:w}") == 0)
		{
			CCLOG("Server response: %s", _cRecvBuffer);
			CCLOG("Updating fireballB position");
			_iFireballBY += 10;
			_FireballB->setPosition(cocos2d::Vec2(_iFireballBX, _iFireballBY));
		}		

		//UpdatePlayerOne(_iPlayerOneX, _iPlayerOneY);
		//UpdatePlayerTwo(_iPlayerTwoX, _iPlayerTwoY);
	}	
}

bool MainScene::onTouchBegan(Touch* touch, Event* event)
{
	return true;
}

void MainScene::onTouchEnded(Touch* touch, Event* event)
{
	// Move FireballA forward on touch
	CCLOG("Updating fireballAs position");
	_iFireballAY += 10;
	_FireballA->setPosition(cocos2d::Vec2(_iFireballAX, _iFireballAY));

	// Send a message to the server about fireballA
	CCLOG("Sending message to the server");
	std::string message = "{uid:001,value:w}";

	sprintf(_cSendBuffer, "%s", message.c_str());
	_NetManager->SendData(_cSendBuffer);
}

void MainScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void MainScene::UpdatePlayerOne(int x, int y)
{
	_PlayerOne->setPosition(cocos2d::Vec2(x, y));
}

void MainScene::UpdatePlayerTwo(int x, int y)
{

}